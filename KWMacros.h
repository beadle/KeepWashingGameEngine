#pragma once


#define SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: varType get##funName(void) const { return varName; }

#define SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: varType get##funName(void) const { return varName; }\
public: void set##funName(varType var){ varName = var; }

#define SYNTHESIZE_RETAIN(varType, varName, funName)    \
private: varType varName; \
public: varType get##funName(void) const { return varName; } \
public: void set##funName(varType var)   \
{ \
	if (varName != var) \
	{ \
	KW_SAFE_RETAIN(var); \
	KW_SAFE_RELEASE(varName); \
	varName = var; \
	} \
} 


#define KW_SAFE_DELETE(p)				do { if(p) { delete (p); (p) = 0; } } while(0)
#define KW_SAFE_DELETE_ARRAY(p)		do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define KW_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)

#define COM_SAFE_RELEASE(p)				do { if(p) { (p)->Release(); } } while(0)
#define COM_SAFE_RELEASE_NULL(p)        do { if(p) { (p)->Release(); (p) = 0; } } while(0)
#define COM_SAFE_RETAIN(p)				do { if(p) { (p)->AddRef(); } } while(0)

#define KW_SAFE_RELEASE(p)				do { if(p) { (p)->Release(); } } while(0)
#define KW_SAFE_RELEASE_NULL(p)        do { if(p) { (p)->Release(); (p) = 0; } } while(0)
#define KW_SAFE_RETAIN(p)				do { if(p) { (p)->Retain(); } } while(0)