/*
** $Id: lobject.h,v 2.117 2016/08/01 19:51:24 roberto Exp $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/


#ifndef lobject_h
#define lobject_h


#include <stdarg.h>


#include "llimits.h"
#include "lua.h"


/*
** Extra tags for non-values
*/
#define LUA_TUPVAL	LUA_NUMTAGS  /* upvalues */
#define LUA_TPROTO	(LUA_NUMTAGS+1)  /* function prototypes */

/*
** number of all possible tags (including LUA_TNONE)
*/
#define LUA_TOTALTAGS	(LUA_TPROTO + 2)


/*
** tags for Tagged Values have the following use of bits:
** bits 0-3: actual tag (a LUA_T* value)
** bits 4-5: variant bits
** bit 6: whether value is collectable
*/


/*
** LUA_TFUNCTION variants:
** 0 - Lua function
** 1 - light C function
** 2 - regular C function (closure)
*/

/* Variant tags for functions */
#define LUA_TLCL	(LUA_TFUNCTION | (0 << 4))  /* Lua closure */
#define LUA_TLCF	(LUA_TFUNCTION | (1 << 4))  /* light C function */
#define LUA_TCCL	(LUA_TFUNCTION | (2 << 4))  /* C closure */


/* Variant tags for strings */
#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))  /* short strings */
#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))  /* long strings */


/* Variant tags for numbers */
#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))  /* float numbers */
#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))  /* integer numbers */
/** RAVI table sunbtypes **/
#define RAVI_TIARRAY (LUA_TTABLE | (1 << 4))  /* Ravi int array */
#define RAVI_TFARRAY (LUA_TTABLE | (2 << 4))  /* Ravi float array */


/* Bit mark for collectable types */
#define BIT_ISCOLLECTABLE	(1 << 7)

/* mark a tag as collectable */
#define ctb(t)			((t) | BIT_ISCOLLECTABLE)


/*
** Common type for all collectable objects
*/
typedef struct GCObject GCObject;


/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
*/
#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked


/*
** Common type has only the common header
*/
struct GCObject {
  CommonHeader;
};




/*
** Tagged Values. This is the basic representation of values in Lua,
** an actual value plus a tag with its type.
*/

/*
** Union of all Lua values
*/
typedef union Value {
  GCObject *gc;    /* collectable objects */
  void *p;         /* light userdata */
  int b;           /* booleans */
  lua_CFunction f; /* light C functions */
  lua_Integer i;   /* integer numbers */
  lua_Number n;    /* float numbers */
} Value;


#define TValuefields	Value value_; lu_byte tt_


typedef struct lua_TValue {
  TValuefields;
} TValue;



/* macro defining a nil value */
#define NILCONSTANT	{NULL}, LUA_TNIL


#define val_(o)		((o)->value_)
#define valraw(o)	(&val_(o))


/* raw type tag of a TValue */
#define rttype(o)	((o)->tt_)

/* tag with no variants (bits 0-3) */
#define novariant(x)	((x) & 0x0F)

/* type tag of a TValue (bits 0-3 for tags + variant bits 4-5) */
#define ttyperaw(t)	((t) & 0x3F)
#define ttype(o)	ttyperaw(rttype(o))

/* type tag of a TValue with no variants (bits 0-3) */
#define ttnov(o)	(novariant(rttype(o)))


/* Macros to test type */
#define checktag(o,t)		(rttype(o) == (t))
#define checktype(o,t)		(ttnov(o) == (t))
#define ttisnumber(o)		checktype((o), LUA_TNUMBER)
#define ttisfloat(o)		checktag((o), LUA_TNUMFLT)
#define ttisinteger(o)		checktag((o), LUA_TNUMINT)
#define ttisnil(o)		checktag((o), LUA_TNIL)
#define ttisboolean(o)		checktag((o), LUA_TBOOLEAN)
#define ttislightuserdata(o)	checktag((o), LUA_TLIGHTUSERDATA)
#define ttisstring(o)		checktype((o), LUA_TSTRING)
#define ttisshrstring(o)	checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o)	checktag((o), ctb(LUA_TLNGSTR))
/* RAVI change: we support two sub types of table type
   and hence need to distinguish between the types.
   ttistable() returns true for all table types
   ttisLtable() only returns true if the value is a Lua table 
   ttisiarray() only returns true if the value is a Ravi subtype integer[]
   ttisfarray() only returns true if the value is a Ravi subtype number[]
 */
#define ttistable(o)		checktype((o), LUA_TTABLE)
#define ttisiarray(o)    checktag((o), ctb(RAVI_TIARRAY))
#define ttisfarray(o)    checktag((o), ctb(RAVI_TFARRAY))
#define ttisLtable(o)    checktag((o), ctb(LUA_TTABLE))
#define ttisfunction(o)		checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)		((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisCclosure(o)		checktag((o), ctb(LUA_TCCL))
#define ttisLclosure(o)		checktag((o), ctb(LUA_TLCL))
#define ttislcf(o)		checktag((o), LUA_TLCF)
#define ttisfulluserdata(o)	checktag((o), ctb(LUA_TUSERDATA))
#define ttisthread(o)		checktag((o), ctb(LUA_TTHREAD))


/*
** Macros to access unstructured values (may come both from
** 'TValue's and table keys)
*/
#define ivalueraw(v)	((v).i)
#define fltvalueraw(v)	((v).n)
#define gcvalueraw(v)	((v).gc)
#define pvalueraw(v)	((v).p)
#define tsvalueraw(v)	(gco2ts((v).gc))
#define fvalueraw(v)	((v).f)
#define bvalueraw(v)	((v).b)


/* Macros to access values */
#define ivalue(o)	check_exp(ttisinteger(o), val_(o).i)
#define fltvalue(o)	check_exp(ttisfloat(o), val_(o).n)
#define nvalue(o)	check_exp(ttisnumber(o), \
	(ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define gcvalue(o)	check_exp(iscollectable(o), val_(o).gc)
#define pvalue(o)	check_exp(ttislightuserdata(o), val_(o).p)
#define tsvalue(o)	check_exp(ttisstring(o), gco2ts(val_(o).gc))
#define uvalue(o)	check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))
#define clvalue(o)	check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o)	check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define clCvalue(o)	check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))
#define fvalue(o)	check_exp(ttislcf(o), val_(o).f)
#define hvalue(o)	check_exp(ttistable(o), gco2t(val_(o).gc))
#define bvalue(o)	check_exp(ttisboolean(o), val_(o).b)
#define thvalue(o)	check_exp(ttisthread(o), gco2th(val_(o).gc))

#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))


#define iscollectable(o)	(rttype(o) & BIT_ISCOLLECTABLE)


/*
** Protected access to objects in values
*/
#define gcvalueN(o)	(iscollectable(o) ? gcvalue(o) : NULL)


/* Macros for internal tests */
#define righttt(obj)		(ttype(obj) == gcvalue(obj)->tt)

#define checkliveness(L,obj) \
	lua_longassert(!iscollectable(obj) || \
		(righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj)))))


/* Macros to set values */
#define settt_(o,t)	((o)->tt_=(t))

#define setfltvalue(obj,x) \
  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_TNUMFLT); }

#define chgfltvalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisfloat(io)); val_(io).n=(x); }

#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_TNUMINT); }

#define chgivalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisinteger(io)); val_(io).i=(x); }

#define setnilvalue(obj) settt_(obj, LUA_TNIL)

#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }

#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }

#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }

#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }

#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(L,io); }

#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TUSERDATA)); \
    checkliveness(L,io); }

#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTHREAD)); \
    checkliveness(L,io); }

#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TLCL)); \
    checkliveness(L,io); }

#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TCCL)); \
    checkliveness(L,io); }

#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_TTABLE)); \
    checkliveness(L,io); }

/** RAVI extension **/
#define setiarrayvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TIARRAY)); \
    checkliveness(L,io); }

/** RAVI extension **/
#define setfarrayvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(RAVI_TFARRAY)); \
    checkliveness(L,io); }

#define setobj(L,obj1,obj2) \
	{ TValue *io1=(obj1); const TValue *io2=(obj2); \
          io1->value_ = io2->value_; io1->tt_ = io2->tt_; \
	  (void)L; checkliveness(L,io1); }


/*
** different types of assignments, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s	setobj
/* to stack (not from same stack) */
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define sethvalue2s	sethvalue
#define setthvalue2s    setthvalue
#define setptvalue2s	setptvalue
#define setclLvalue2s   setclLvalue
/* from table to same table */
#define setobjt2t	setobj
/* to new object */
#define setobj2n	setobj
#define setsvalue2n	setsvalue
/* to table */
#define setobj2t	setobj




/*
** {======================================================
** types and prototypes
** =======================================================
*/


typedef TValue *StkId;  /* index to stack elements */




/*
** Header for string value; string bytes follow the end of this structure
** (aligned according to 'UTString'; see next).
*/
typedef struct TString {
  CommonHeader;
  lu_byte extra;  /* reserved words for short strings; "has hash" for longs */
  lu_byte shrlen;  /* length for short strings */
  unsigned int hash;
  union {
    size_t lnglen;  /* length for long strings */
    struct TString *hnext;  /* linked list for hash table */
  } u;
} TString;


/*
** Ensures that address after this type is always fully aligned.
*/
typedef union UTString {
  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  TString tsv;
} UTString;


/*
** Get the actual string (array of bytes) from a 'TString'.
** (Access to 'extra' ensures that value is really a 'TString'.)
*/
#define getstr(ts)  \
  check_exp(sizeof((ts)->extra), cast(char *, (ts)) + sizeof(UTString))


/* get the actual string (array of bytes) from a Lua value */
#define svalue(o)       getstr(tsvalue(o))

/* get string length from 'TString *s' */
#define tsslen(s)	((s)->tt == LUA_TSHRSTR ? (s)->shrlen : (s)->u.lnglen)

/* get string length from 'TValue *o' */
#define vslen(o)	tsslen(tsvalue(o))


/*
** Header for userdata; memory area follows the end of this structure
** (aligned according to 'UUdata'; see next).
*/
typedef struct Udata {
  CommonHeader;
  lu_byte ttuv_;  /* user value's tag */
  struct Table *metatable;
  size_t len;  /* number of bytes */
  union Value user_;  /* user value */
} Udata;


/*
** Ensures that address after this type is always fully aligned.
*/
typedef union UUdata {
  L_Umaxalign dummy;  /* ensures maximum alignment for 'local' udata */
  Udata uv;
} UUdata;


/*
**  Get the address of memory block inside 'Udata'.
** (Access to 'ttuv_' ensures that value is really a 'Udata'.)
*/
#define getudatamem(u)  \
  check_exp(sizeof((u)->ttuv_), (cast(char*, (u)) + sizeof(UUdata)))

#define setuservalue(L,u,o) \
	{ const TValue *io=(o); Udata *iu = (u); \
	  iu->user_ = io->value_; iu->ttuv_ = rttype(io); \
	  checkliveness(L,io); }


#define getuservalue(L,u,o) \
	{ TValue *io=(o); const Udata *iu = (u); \
	  io->value_ = iu->user_; settt_(io, iu->ttuv_); \
	  checkliveness(L,io); }

/* RAVI: Following are the types we will use
** use in parsing. The rationale for types is
** performance - as of now these are the only types that
** we care about from a performance point of view - if any
** other types appear then they are all treated as ANY
**/
typedef enum {
  RAVI_TANY = 0,      /* Lua dynamic type */
  RAVI_TNUMINT = 1,    /* integer number */
  RAVI_TNUMFLT,        /* floating point number */
  RAVI_TARRAYINT,      /* array of ints */
  RAVI_TARRAYFLT,      /* array of doubles */
  RAVI_TFUNCTION,      /* Lua or C Function */
  RAVI_TTABLE,         /* Lua table */
  RAVI_TSTRING,        /* string */
  RAVI_TNIL,           /* NIL */
  RAVI_TBOOLEAN,       /* boolean */
  RAVI_TUSERDATA       /* userdata or lightuserdata */
} ravitype_t;

/*
** Description of an upvalue for function prototypes
*/
typedef struct Upvaldesc {
  TString *name;  /* upvalue name (for debug information) */
  TString *usertype; /* RAVI extension: name of user type */
  lu_byte ravi_type; /* RAVI type of upvalue */
  lu_byte instack;  /* whether it is in stack (register) */
  lu_byte idx;  /* index of upvalue (in stack or in outer function's list) */
} Upvaldesc;


/*
** Description of a local variable for function prototypes
** (used for debug information)
*/
typedef struct LocVar {
  TString *varname;
  TString *usertype; /* RAVI extension: name of user type */
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
  lu_byte ravi_type; /* RAVI type of the variable - RAVI_TANY if unknown */
} LocVar;

/** RAVI changes start */
typedef enum {
  RAVI_JIT_NOT_COMPILED = 0,
  RAVI_JIT_CANT_COMPILE = 1,
  RAVI_JIT_COMPILED = 2 /* But actual function pointer must be checked */
} ravi_jit_status_t;

typedef enum {
  RAVI_JIT_FLAG_NONE = 0,
  RAVI_JIT_FLAG_HASFORLOOP = 1
} ravi_jit_flag_t;

typedef struct RaviJITProto {
  lu_byte jit_status; /* 0=not compiled, 1=can't compile */
  lu_byte jit_flags; 
  unsigned short execution_count;   /* how many times has function been executed */
  void *jit_data;
  lua_CFunction jit_function;
} RaviJITProto;
/** RAVI changes end */

/*
** Function Prototypes
*/
typedef struct Proto {
  CommonHeader;
  lu_byte numparams;  /* number of fixed parameters */
  lu_byte is_vararg;
  lu_byte maxstacksize;  /* number of registers needed by this function */
  int sizeupvalues;  /* size of 'upvalues' */
  int sizek;  /* size of 'k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of 'p' */
  int sizelocvars;
  int linedefined;  /* debug information  */
  int lastlinedefined;  /* debug information  */
  TValue *k;  /* constants used by the function */
  Instruction *code;  /* opcodes */
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines (debug information) */
  LocVar *locvars;  /* information about local variables (debug information) */
  Upvaldesc *upvalues;  /* upvalue information */
  struct LClosure *cache;  /* last-created closure with this prototype */
  TString  *source;  /* used for debug information */
  GCObject *gclist;
  lu_byte cachemiss;  /* count for successive misses for 'cache' field */
  /* RAVI extension */
  RaviJITProto ravi_jit;
} Proto;



/*
** Upvalues for Lua closures
*/
typedef struct UpVal {
  CommonHeader;
  TValue *v;  /* points to stack or to its own value */
  union {
    struct {  /* (when open) */
      struct UpVal *next;  /* linked list */
      struct UpVal **previous;
    } open;
    TValue value;  /* the value (when closed) */
  } u;
} UpVal;



/*
** Closures
*/

#define ClosureHeader \
	CommonHeader; lu_byte nupvalues; GCObject *gclist

typedef struct CClosure {
  ClosureHeader;
  lua_CFunction f;
  TValue upvalue[1];  /* list of upvalues */
} CClosure;


typedef struct LClosure {
  ClosureHeader;
  struct Proto *p;
  UpVal *upvals[1];  /* list of upvalues */
} LClosure;


typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;


#define isLfunction(o)	ttisLclosure(o)

#define getproto(o)	(clLvalue(o)->p)


/*
** Tables
*/


/*
** Nodes for Hash tables. A pack of two TValue's (key-value pairs)
** plus a 'next' field to link colliding entries. The distribution
** of the key's fields ('key_tt' and 'key_val') not forming a proper
** 'TValue' allows for a smaller size for 'Node' both in 4-byte
** and 8-byte alignments.
*/
typedef union Node {
  struct NodeKey {
    TValuefields;  /* fields for value */
    lu_byte key_tt;  /* key type */
    int next;  /* for chaining */
    Value key_val;  /* key value */
  } u;
  TValue i_val;  /* direct access to node's value as a proper 'TValue' */
} Node;


/* copy a value into a key */
#define setnodekey(L,node,obj) \
	{ Node *n_=(node); const TValue *io_=(obj); \
	  n_->u.key_val = io_->value_; n_->u.key_tt = io_->tt_; \
	  (void)L; checkliveness(L,io_); }


/* copy a value from a key */
#define getnodekey(L,obj,node) \
	{ TValue *io_=(obj); const Node *n_=(node); \
	  io_->value_ = n_->u.key_val; io_->tt_ = n_->u.key_tt; \
	  (void)L; checkliveness(L,io_); }


/** RAVI extension */
typedef enum RaviArrayModifer {
  RAVI_ARRAY_SLICE = 1,
  RAVI_ARRAY_FIXEDSIZE = 2
} RaviArrayModifier;

/** RAVI extension */
typedef struct RaviArray {
  char *data;
  unsigned int len; /* RAVI len specialization */
  unsigned int size; /* amount of memory allocated */
  lu_byte array_type; /* RAVI specialization */
  lu_byte array_modifier; /* Flags that affect how the array is handled */
} RaviArray;

typedef struct Table {
  CommonHeader;
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  lu_byte lsizenode;  /* log2 of size of 'node' array */
  unsigned int sizearray;  /* size of 'array' array */
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  struct Table *metatable;
  GCObject *gclist;
  /** RAVI extension */
  RaviArray ravi_array;
#if RAVI_USE_NEWHASH
  // TODO we should reorganize this structure
  unsigned int hmask; /* Hash part mask (size of hash part - 1) - borrowed from LuaJIT */
#endif
} Table;


/*
** Macros to manipulate keys inserted in nodes
*/
#define keytt(node)		((node)->u.key_tt)
#define keyval(node)		((node)->u.key_val)

#define keyisnil(node)		(keytt(node) == LUA_TNIL)
#define keyisinteger(node)	(keytt(node) == LUA_TNUMINT)
#define keyival(node)		(keyval(node).i)
#define keyisshrstr(node)	(keytt(node) == ctb(LUA_TSHRSTR))
#define keystrval(node)		(gco2ts(keyval(node).gc))

#define setnilkey(node)		(keytt(node) = LUA_TNIL)

#define keyiscollectable(n)	(keytt(n) & BIT_ISCOLLECTABLE)

#define gckey(n)	(keyval(n).gc)
#define gckeyN(n)	(keyiscollectable(n) ? gckey(n) : NULL)


/*
** Use a "nil table" to mark dead keys in a table. Those keys serve
** only to keep space for removed entries, which may still be part of
** chains. Note that the 'keytt' does not have the BIT_ISCOLLECTABLE
** set, so these values are considered not collectable and are different
** from any valid value.
*/
#define setdeadkey(n)	(keytt(n) = LUA_TTABLE, gckey(n) = NULL)



/*
** 'module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
	(check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))


#define twoto(x)	(1<<(x))
#define sizenode(t)	(twoto((t)->lsizenode))


/*
** (address of) a fixed nil value
*/
#define luaO_nilobject		(&luaO_nilobject_)

/* Internal assembler functions. Never call these directly from C. */
typedef void(*ASMFunction)(void);


LUAI_DDEC const TValue luaO_nilobject_;

/* size of buffer for 'luaO_utf8esc' function */
#define UTF8BUFFSZ	8

LUAI_FUNC int luaO_int2fb (unsigned int x);
LUAI_FUNC int luaO_fb2int (int x);
LUAI_FUNC int luaO_utf8esc (char *buff, unsigned long x);
LUAI_FUNC int luaO_ceillog2 (unsigned int x);
LUAI_FUNC void luaO_arith (lua_State *L, int op, const TValue *p1,
                           const TValue *p2, TValue *res);
LUAI_FUNC size_t luaO_str2num (const char *s, TValue *o);
LUAI_FUNC int luaO_hexavalue (int c);
LUAI_FUNC void luaO_tostring (lua_State *L, StkId obj);
LUAI_FUNC const char *luaO_pushvfstring (lua_State *L, const char *fmt,
                                                       va_list argp);
LUAI_FUNC const char *luaO_pushfstring (lua_State *L, const char *fmt, ...);
LUAI_FUNC void luaO_chunkid (char *out, const char *source, size_t len);


#endif

