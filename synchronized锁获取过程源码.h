1.BasicObjectLock类源码

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
 
 
 2.basicLock.hpp中BasicLock源码
 
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
 
 
