#ifndef _PCLINT_H_
#define _PCLINT_H_

//pclint接口

//////////////////////////////////////////////////////////////////////////
//检查规则
//用检查func_template的规则去检查func
#define LINT_LIKE_FUNC(func_template, func)	/*lint --function(func_template, func) */

//允许类的接口func托管指针
#define LINT_CUSTODIAL(func)				/*-sem(func,custodial(t)) */

//允许模板t的接口func托管指针
#define LINT_CUSTODIAL_TEMPLATE(t, func)	LINT_LIKE_FUNC(free, t::func);LINT_NOERR_CALLFUNC(424, t<*>::func)

#define TEST_ME(func)

//////////////////////////////////////////////////////////////////////////
//隐藏告警

//隐藏调用函数func时产生的告警
#define LINT_NOERR_CALLFUNC(err, func)	/*lint -ecall(err, func) */

//隐藏调用符号时产生的告警
#define LINT_NOERR_SYM(err, sym)	/*lint -esym(err, sym) */

//隐藏当前行产生的告警
#define LINT_NOERR_LINE(err)			/*line !e{err} */

//隐藏函数内的告警
#define LINT_NOERR_INFUNC(err, func)	/*line -efunc(err,func) */

//////////////////////////////////////////////////////////////////////////


#endif