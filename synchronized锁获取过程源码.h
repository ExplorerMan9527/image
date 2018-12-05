1.InterpreterRuntime.cpp的InterpreterRuntime::monitorenter

IRT_ENTRY_NO_ASYNC(void, InterpreterRuntime::monitorenter(JavaThread* thread, BasicObjectLock* elem))
 #ifdef ASSERT0
   thread->last_frame().interpreter_frame_verify_monitor(elem);
 #endif
   if (PrintBiasedLockingStatistics) {
     Atomic::inc(BiasedLocking::slow_path_entry_count_addr());
   }
   Handle h_obj(thread, elem->obj());
   assert(Universe::heap()->is_in_reserved_or_null(h_obj()),
          "must be NULL or an object");
   if (UseBiasedLocking) {//标识虚拟机是否开启偏向锁功能,默认开启
     // Retry fast entry if bias is revoked to avoid unnecessary inflation
     ObjectSynchronizer::fast_enter(h_obj, elem->lock(), true, CHECK);
   } else {
     ObjectSynchronizer::slow_enter(h_obj, elem->lock(), CHECK);
   }
   assert(Universe::heap()->is_in_reserved_or_null(elem->obj()),
          "must be NULL or an object");
 #ifdef ASSERT
   thread->last_frame().interpreter_frame_verify_monitor(elem);
 #endif
 IRT_END

---------------------------------- InterpreterRuntime::monitorenter ---------------------------------------------------------------------

2.BasicObjectLock类源码

class BasicObjectLock VALUE_OBJ_CLASS_SPEC {
   friend class VMStructs;
  private:
  
   BasicLock _lock; //  BasicLock类型_lock对象主要用来保存：指向Object对象的对象头数据；
   oop       _obj; // 指向Object对象的指针；
 
  public:
   // Manipulation
   oop      obj() const                                { return _obj;  }
   void set_obj(oop obj)                               { _obj = obj; }
   BasicLock* lock()                                   { return &_lock; }
 
   // Note: Use frame::interpreter_frame_monitor_size() for the size of BasicObjectLocks
   //       in interpreter activation frames since it includes machine-specific padding.
   static int size()       { return sizeof(BasicObjectLock)/wordSize; }
 
   // GC support
   void oops_do(OopClosure* f) { f->do_oop(&_obj); }
 
   static int obj_offset_in_bytes()   { return offset_of(BasicObjectLock, _obj);  }
   static int lock_offset_in_bytes()   { return offset_of(BasicObjectLock, _lock); }
 };
 -------------------------------------BasicObjectLock---------------------------------------------------------------------------
 
 3.basicLock.hpp中BasicLock源码
 
 class BasicLock VALUE_OBJ_CLASS_SPEC {
   friend class VMStructs;
  private:
	//markOop是不是很熟悉,对象头
   volatile markOop _displaced_header;
  public:
   markOop      displaced_header() const               { return _displaced_header; }
   void         set_displaced_header(markOop header)   { _displaced_header = header; }
 
   void print_on(outputStream* st) const;
 
   // move a basic lock (used during deoptimization
   void move_to(oop obj, BasicLock* dest);
 
   static int displaced_header_offset_in_bytes(){ 
		return offset_of(BasicLock, _displaced_header); }
 };
 
 -----------------------------------BasicLock源码--------------------------------------------------------------------------
 4.ObjectSynchronizer::fast_enter方法源码
 
 void ObjectSynchronizer::fast_enter(Handle obj, BasicLock* lock, bool attempt_rebias, TRAPS) {
  //是否开启使用偏向锁
  if (UseBiasedLocking) {
    //是否到达安全点
     if (!SafepointSynchronize::is_at_safepoint()) {
       //获取偏向锁
       BiasedLocking::Condition cond = BiasedLocking::revoke_and_rebias(obj, attempt_rebias, THREAD);
       if (cond == BiasedLocking::BIAS_REVOKED_AND_REBIASED) {
         return;
       }
     } else {
       assert(!attempt_rebias, "can not rebias toward VM thread");
       BiasedLocking::revoke_at_safepoint(obj);
     }
     assert(!obj->mark()->has_bias_pattern(), "biases should be revoked by now");
  }
  //获取轻量级锁
  slow_enter (obj, lock, THREAD) ;
 }
--------------------------------------ObjectSynchronizer::fast_enter--------------------------------------------------
