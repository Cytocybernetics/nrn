/* Created by Language version: 6.2.0 */
/* VECTORIZED */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "corebluron/mech/cfile/scoplib.h"
#undef PI
#ifdef _PROF_HPM 
void HPM_Start(const char *); 
void HPM_Stop(const char *); 
#endif 
 
#include "corebluron/nrnoc/md1redef.h"
#include "corebluron/nrnconf.h"
#include "corebluron/nrnoc/multicore.h"

#include "corebluron/nrnoc/md2redef.h"
#if METHOD3
extern int _method3;
#endif

#if !NRNGPU
#undef exp
#define exp hoc_Exp
extern double hoc_Exp(double);
#endif
 
#define nrn_init _nrn_init__StochKv
#define nrn_cur _nrn_cur__StochKv
#define _nrn_current _nrn_current__StochKv
#define nrn_jacob _nrn_jacob__StochKv
#define nrn_state _nrn_state__StochKv
#define _net_receive _net_receive__StochKv 
#define ChkProb ChkProb__StochKv 
#define _f_trates _f_trates__StochKv 
#define setRNG setRNG__StochKv 
#define states states__StochKv 
#define trates trates__StochKv 
 
#define _threadargscomma_ _p, _ppvar, _thread, _nt,
#define _threadargsprotocomma_ double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt,
#define _threadargs_ _p, _ppvar, _thread, _nt
#define _threadargsproto_ double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt
 	/*SUPPRESS 761*/
	/*SUPPRESS 762*/
	/*SUPPRESS 763*/
	/*SUPPRESS 765*/
	 extern double *getarg();
 /* Thread safe. No static _p or _ppvar. */
 
#define t _nt->_t
#define dt _nt->_dt
#define gamma _p[0]
#define eta _p[1]
#define gkbar _p[2]
#define a _p[3]
#define b _p[4]
#define ik _p[5]
#define gk _p[6]
#define ninf _p[7]
#define ntau _p[8]
#define tadj _p[9]
#define N _p[10]
#define P_a _p[11]
#define P_b _p[12]
#define N0 _p[13]
#define N1 _p[14]
#define n0_n1 _p[15]
#define n1_n0 _p[16]
#define n _p[17]
#define ek _p[18]
#define scale_dens _p[19]
#define n0_n1_new _p[20]
#define Dn _p[21]
#define v _p[22]
#define _g _p[23]
#define _ion_ek		_nt->_data[_ppvar[0]]
#define _ion_ik	_nt->_data[_ppvar[1]]
#define _ion_dikdv	_nt->_data[_ppvar[2]]
#define _p_rng	_nt->_vdata[_ppvar[3]]
#define area	_nt->_data[_ppvar[4]]
 
#if MAC
#if !defined(v)
#define v _mlhv
#endif
#if !defined(h)
#define h _mlhh
#endif
#endif
 
#if defined(__cplusplus)
extern "C" {
#endif
 static int hoc_nrnpointerindex =  3;
 static ThreadDatum* _extcall_thread;
 /* external NEURON variables */
 extern double celsius;
 
#if 0 /*BBCORE*/
 /* declaration of user functions */
 static void _hoc_BnlDev(void);
 static void _hoc_ChkProb(void);
 static void _hoc_SigmoidRate(void);
 static void _hoc_brand(void);
 static void _hoc_setRNG(void);
 static void _hoc_strap(void);
 static void _hoc_trates(void);
 static void _hoc_urand(void);
 
#endif /*BBCORE*/
 static int _mechtype;
extern int nrn_get_mechtype();
extern void hoc_register_prop_size(int, int, int);
extern Memb_func* memb_func;
 
#if 0 /*BBCORE*/
 /* connect user functions to hoc names */
 static VoidFunc hoc_intfunc[] = {
 "setdata_StochKv", _hoc_setdata,
 "BnlDev_StochKv", _hoc_BnlDev,
 "ChkProb_StochKv", _hoc_ChkProb,
 "SigmoidRate_StochKv", _hoc_SigmoidRate,
 "brand_StochKv", _hoc_brand,
 "setRNG_StochKv", _hoc_setRNG,
 "strap_StochKv", _hoc_strap,
 "trates_StochKv", _hoc_trates,
 "urand_StochKv", _hoc_urand,
 0, 0
};
 
#endif /*BBCORE*/
#define BnlDev BnlDev_StochKv
#define SigmoidRate SigmoidRate_StochKv
#define brand brand_StochKv
#define strap strap_StochKv
#define urand urand_StochKv
 extern double BnlDev( _threadargsprotocomma_ double , double );
 extern double SigmoidRate( _threadargsprotocomma_ double , double , double , double );
 extern double brand( _threadargsprotocomma_ double , double );
 extern double strap( _threadargsprotocomma_ double );
 extern double urand( _threadargsproto_ );
 
static void _check_trates(double*, Datum*, ThreadDatum*, _NrnThread*); 
static void _check_table_thread(double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt, int _type) {
   _check_trates(_p, _ppvar, _thread, _nt);
 }
 /* declare global and static user variables */
#define Rb Rb_StochKv
 double Rb = 0.002;
#define Ra Ra_StochKv
 double Ra = 0.02;
#define deterministic deterministic_StochKv
 double deterministic = 0;
#define qa qa_StochKv
 double qa = 9;
#define q10 q10_StochKv
 double q10 = 2.3;
#define tha tha_StochKv
 double tha = -40;
#define temp temp_StochKv
 double temp = 23;
#define usetable usetable_StochKv
 double usetable = 1;
#define vmax vmax_StochKv
 double vmax = 100;
#define vmin vmin_StochKv
 double vmin = -120;
 
#if 0 /*BBCORE*/
 /* some parameters have upper and lower limits */
 static HocParmLimits _hoc_parm_limits[] = {
 "usetable_StochKv", 0, 1,
 0,0,0
};
 static HocParmUnits _hoc_parm_units[] = {
 "tha_StochKv", "mV",
 "Ra_StochKv", "/ms",
 "Rb_StochKv", "/ms",
 "temp_StochKv", "degC",
 "vmin_StochKv", "mV",
 "vmax_StochKv", "mV",
 "gamma_StochKv", "pS",
 "eta_StochKv", "1/um2",
 "gkbar_StochKv", "S/cm2",
 "a_StochKv", "/ms",
 "b_StochKv", "/ms",
 "ik_StochKv", "mA/cm2",
 "gk_StochKv", "S/cm2",
 "ntau_StochKv", "ms",
 0,0
};
 
#endif /*BBCORE*/
 static double delta_t = 1;
 static double n0 = 0;
 
#if 0 /*BBCORE*/
 /* connect global user variables to hoc */
 static DoubScal hoc_scdoub[] = {
 "tha_StochKv", &tha_StochKv,
 "qa_StochKv", &qa_StochKv,
 "Ra_StochKv", &Ra_StochKv,
 "Rb_StochKv", &Rb_StochKv,
 "temp_StochKv", &temp_StochKv,
 "q10_StochKv", &q10_StochKv,
 "deterministic_StochKv", &deterministic_StochKv,
 "vmin_StochKv", &vmin_StochKv,
 "vmax_StochKv", &vmax_StochKv,
 "usetable_StochKv", &usetable_StochKv,
 0,0
};
 static DoubVec hoc_vdoub[] = {
 0,0,0
};
 
#endif /*BBCORE*/
 static double _sav_indep;
 static void nrn_alloc(double*, Datum*, int);
static void  nrn_init(_NrnThread*, _Memb_list*, int);
static void nrn_state(_NrnThread*, _Memb_list*, int);
 static void nrn_cur(_NrnThread*, _Memb_list*, int);
static void  nrn_jacob(_NrnThread*, _Memb_list*, int);
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "6.2.0",
"StochKv",
 "gamma_StochKv",
 "eta_StochKv",
 "gkbar_StochKv",
 0,
 "a_StochKv",
 "b_StochKv",
 "ik_StochKv",
 "gk_StochKv",
 "ninf_StochKv",
 "ntau_StochKv",
 "tadj_StochKv",
 "N_StochKv",
 "P_a_StochKv",
 "P_b_StochKv",
 "N0_StochKv",
 "N1_StochKv",
 "n0_n1_StochKv",
 "n1_n0_StochKv",
 0,
 "n_StochKv",
 0,
 "rng_StochKv",
 0};
 static int _k_type;
 
static void nrn_alloc(double* _p, Datum* _ppvar, int _type) {
 	/*initialize range parameters*/
 	gamma = 30;
 	eta = 0;
 	gkbar = 0.75;
 
#if 0 /*BBCORE*/
 prop_ion = need_memb(_k_sym);
 nrn_promote(prop_ion, 0, 1);
 	_ppvar[0]._pval = &prop_ion->param[0]; /* ek */
 	_ppvar[1]._pval = &prop_ion->param[3]; /* ik */
 	_ppvar[2]._pval = &prop_ion->param[4]; /* _ion_dikdv */
 
#endif /* BBCORE */
 
}
 static void _initlists();
 static void _update_ion_pointer(Datum*);
 
#define _psize 24
#define _ppsize 5
 static void bbcore_read(double *, int*, int*, int*, _threadargsproto_);
 extern void hoc_reg_bbcore_read(int, void(*)(double *, int*, int*, int*, _threadargsproto_));
 extern Symbol* hoc_lookup(const char*);
extern void _nrn_thread_reg(int, int, void(*f)(Datum*));
extern void _nrn_thread_table_reg(int, void(*)(double*, Datum*, ThreadDatum*, _NrnThread*, int));
extern void _cvode_abstol( Symbol**, double*, int);

 void _StochKv_reg() {
	int _vectorized = 1;
  _initlists();
 _k_type = nrn_get_mechtype("k_ion"); 
#if 0 /*BBCORE*/
 	ion_reg("k", -10000.);
 	_k_sym = hoc_lookup("k_ion");
 
#endif /*BBCORE*/
 	register_mech(_mechanism, nrn_alloc,nrn_cur, nrn_jacob, nrn_state, nrn_init, hoc_nrnpointerindex, 1);
 _mechtype = nrn_get_mechtype(_mechanism[1]);
     _nrn_thread_table_reg(_mechtype, _check_table_thread);
   hoc_reg_bbcore_read(_mechtype, bbcore_read);
  hoc_register_prop_size(_mechtype, _psize, _ppsize);
 }
 static double *_t_ntau;
 static double *_t_ninf;
 static double *_t_a;
 static double *_t_b;
 static double *_t_tadj;
static char *modelname = "skm95.mod  ";

static int error;
static int _ninits = 0;
static int _match_recurse=1;
static void _modl_cleanup(){ _match_recurse=1;}
static int ChkProb(_threadargsprotocomma_ double);
static int _f_trates(_threadargsprotocomma_ double);
static int setRNG(_threadargsproto_);
static int trates(_threadargsprotocomma_ double);
 
static int _ode_spec1(_threadargsproto_);
static int _ode_matsol1(_threadargsproto_);
 static void _n_trates(_threadargsprotocomma_ double _lv);
 static int _slist1[1], _dlist1[1];
 static int states(_threadargsproto_);
 
/*VERBATIM*/
#include "nrnran123.h"
extern int cvode_active_;
 
/*CVODE*/
 static int _ode_spec1 (double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt) {int _reset = 0; {
   trates ( _threadargscomma_ v ) ;
   Dn = a - ( a + b ) * n ;
   if ( deterministic  || dt > 1.0 ) {
     N1 = n * N ;
     if (  ! deterministic ) {
       N1 = floor ( N1 + 0.5 ) ;
       }
     }
   else {
     P_a = strap ( _threadargscomma_ a * dt ) ;
     P_b = strap ( _threadargscomma_ b * dt ) ;
     ChkProb ( _threadargscomma_ P_a ) ;
     ChkProb ( _threadargscomma_ P_b ) ;
     n0_n1 = BnlDev ( _threadargscomma_ P_a , N0 ) ;
     n1_n0 = BnlDev ( _threadargscomma_ P_b , N1 ) ;
     N0 = strap ( _threadargscomma_ N0 - n0_n1 + n1_n0 ) ;
     N1 = N - N0 ;
     }
   N0 = N - N1 ;
   }
 return _reset;
}
 static int _ode_matsol1 (double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt) {
 trates ( _threadargscomma_ v ) ;
 Dn = Dn  / (1. - dt*( ( - (( a + b ))*(1.0) ) )) ;
 return 0;
}
 /*END CVODE*/
 static int states (double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt) { {
   trates ( _threadargscomma_ v ) ;
    n = n + (1. - exp(dt*(( - (( a + b ))*(1.0) ))))*(- ( a ) / ( ( - (( a + b ))*(1.0)) ) - n) ;
   if ( deterministic  || dt > 1.0 ) {
     N1 = n * N ;
     if (  ! deterministic ) {
       N1 = floor ( N1 + 0.5 ) ;
       }
     }
   else {
     P_a = strap ( _threadargscomma_ a * dt ) ;
     P_b = strap ( _threadargscomma_ b * dt ) ;
     ChkProb ( _threadargscomma_ P_a ) ;
     ChkProb ( _threadargscomma_ P_b ) ;
     n0_n1 = BnlDev ( _threadargscomma_ P_a , N0 ) ;
     n1_n0 = BnlDev ( _threadargscomma_ P_b , N1 ) ;
     N0 = strap ( _threadargscomma_ N0 - n0_n1 + n1_n0 ) ;
     N1 = N - N0 ;
     }
   N0 = N - N1 ;
   }
  return 0;
}
 static double _mfac_trates, _tmin_trates;
  static void _check_trates(double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt) {
  static int _maktable=1; int _i, _j, _ix = 0;
  double _xi, _tmax;
  static double _sav_dt;
  static double _sav_Ra;
  static double _sav_Rb;
  static double _sav_tha;
  static double _sav_qa;
  static double _sav_q10;
  static double _sav_temp;
  static double _sav_celsius;
  if (!usetable) {return;}
  if (_sav_dt != dt) { _maktable = 1;}
  if (_sav_Ra != Ra) { _maktable = 1;}
  if (_sav_Rb != Rb) { _maktable = 1;}
  if (_sav_tha != tha) { _maktable = 1;}
  if (_sav_qa != qa) { _maktable = 1;}
  if (_sav_q10 != q10) { _maktable = 1;}
  if (_sav_temp != temp) { _maktable = 1;}
  if (_sav_celsius != celsius) { _maktable = 1;}
  if (_maktable) { double _x, _dx; _maktable=0;
   _tmin_trates =  vmin ;
   _tmax =  vmax ;
   _dx = (_tmax - _tmin_trates)/199.; _mfac_trates = 1./_dx;
   for (_i=0, _x=_tmin_trates; _i < 200; _x += _dx, _i++) {
    _f_trates(_p, _ppvar, _thread, _nt, _x);
    _t_ntau[_i] = ntau;
    _t_ninf[_i] = ninf;
    _t_a[_i] = a;
    _t_b[_i] = b;
    _t_tadj[_i] = tadj;
   }
   _sav_dt = dt;
   _sav_Ra = Ra;
   _sav_Rb = Rb;
   _sav_tha = tha;
   _sav_qa = qa;
   _sav_q10 = q10;
   _sav_temp = temp;
   _sav_celsius = celsius;
  }
 }

 static int trates(double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt, double _lv) { 
#if 0
_check_trates(_p, _ppvar, _thread, _nt);
#endif
 _n_trates(_p, _ppvar, _thread, _nt, _lv);
 return 0;
 }

 static void _n_trates(double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt, double _lv){ int _i, _j;
 double _xi, _theta;
 if (!usetable) {
 _f_trates(_p, _ppvar, _thread, _nt, _lv); return; 
}
 _xi = _mfac_trates * (_lv - _tmin_trates);
 if (isnan(_xi)) {
  ntau = _xi;
  ninf = _xi;
  a = _xi;
  b = _xi;
  tadj = _xi;
  return;
 }
 if (_xi <= 0.) {
 ntau = _t_ntau[0];
 ninf = _t_ninf[0];
 a = _t_a[0];
 b = _t_b[0];
 tadj = _t_tadj[0];
 return; }
 if (_xi >= 199.) {
 ntau = _t_ntau[199];
 ninf = _t_ninf[199];
 a = _t_a[199];
 b = _t_b[199];
 tadj = _t_tadj[199];
 return; }
 _i = (int) _xi;
 _theta = _xi - (double)_i;
 ntau = _t_ntau[_i] + _theta*(_t_ntau[_i+1] - _t_ntau[_i]);
 ninf = _t_ninf[_i] + _theta*(_t_ninf[_i+1] - _t_ninf[_i]);
 a = _t_a[_i] + _theta*(_t_a[_i+1] - _t_a[_i]);
 b = _t_b[_i] + _theta*(_t_b[_i+1] - _t_b[_i]);
 tadj = _t_tadj[_i] + _theta*(_t_tadj[_i+1] - _t_tadj[_i]);
 }

 
static int  _f_trates ( _threadargsprotocomma_ double _lv ) {
   tadj = pow( q10 , ( ( celsius - temp ) / ( 10.0 ) ) ) ;
   a = SigmoidRate ( _threadargscomma_ _lv , tha , Ra , qa ) ;
   a = a * tadj ;
   b = SigmoidRate ( _threadargscomma_ - _lv , - tha , Rb , qa ) ;
   b = b * tadj ;
   ntau = 1.0 / ( a + b ) ;
   ninf = a * ntau ;
    return 0; }
 
#if 0 /*BBCORE*/
 
static void _hoc_trates(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 
#if 1
 _check_trates(_p, _ppvar, _thread, _nt);
#endif
 _r = 1.;
 trates ( _p, _ppvar, _thread, _nt, *getarg(1) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
double SigmoidRate ( _threadargsprotocomma_ double _lv , double _lth , double _la , double _lq ) {
   double _lSigmoidRate;
  if ( fabs ( _lv - _lth ) > 1e-6 ) {
     _lSigmoidRate = _la * ( _lv - _lth ) / ( 1.0 - exp ( - ( _lv - _lth ) / _lq ) ) ;
      }
   else {
     _lSigmoidRate = _la * _lq ;
     }
   
return _lSigmoidRate;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_SigmoidRate(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  SigmoidRate ( _p, _ppvar, _thread, _nt, *getarg(1) , *getarg(2) , *getarg(3) , *getarg(4) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
double strap ( _threadargsprotocomma_ double _lx ) {
   double _lstrap;
 if ( _lx < 0.0 ) {
     _lstrap = 0.0 ;
     
/*VERBATIM*/
        fprintf (stderr,"skv.mod:strap: negative state at time %lf\n", t);
 }
   else {
     _lstrap = _lx ;
     }
   
return _lstrap;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_strap(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  strap ( _p, _ppvar, _thread, _nt, *getarg(1) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
static int  ChkProb ( _threadargsprotocomma_ double _lp ) {
   if ( _lp < 0.0  || _lp > 1.0 ) {
     
/*VERBATIM*/
    fprintf(stderr, "StochKv.mod:ChkProb: argument not a probability.\n");
 }
    return 0; }
 
#if 0 /*BBCORE*/
 
static void _hoc_ChkProb(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r = 1.;
 ChkProb ( _p, _ppvar, _thread, _nt, *getarg(1) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
static int  setRNG ( _threadargsproto_ ) {
   
/*VERBATIM*/
    {
#if !defined(NRNBBCORE) || !NRNBBCORE
	nrnran123_State** pv = (nrnran123_State**)(&_p_rng);
	uint32_t a2 = 0;
	if (*pv) {
		nrnran123_deletestream(*pv);
		*pv = (nrnran123_State*)0;
	} 
	if (ifarg(2)) {
		a2 = (uint32_t)*getarg(2);
	}
	if (ifarg(1)) {
		*pv = nrnran123_newstream((uint32_t)*getarg(1), a2);
	}
#endif
    }
  return 0; }
 
#if 0 /*BBCORE*/
 
static void _hoc_setRNG(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r = 1.;
 setRNG ( _p, _ppvar, _thread, _nt ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
double urand ( _threadargsproto_ ) {
   double _lurand;
 
/*VERBATIM*/
	double value;
	if (_p_rng) {
		/*
		:Supports separate independent but reproducible streams for
		: each instance.
	        : distribution MUST be set to Random.uniform(0,1)
		*/
		value = nrnran123_dblpick((nrnran123_State*)_p_rng);
		//printf("random stream for this simulation = %lf\n",value);
		return value;
	}else{
		//assert(0);
		value = 0.0;
	}
	_lurand = value;
 
return _lurand;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_urand(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  urand ( _p, _ppvar, _thread, _nt ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
/*VERBATIM*/
static void bbcore_write(double* x, int* d, int* xx, int* offset, _threadargsproto_) {
	if (d) {
		uint32_t* di = ((uint32_t*)d) + *offset;
	  // temporary just enough to see how much space is being used
	  if (!_p_rng) {
		di[0] = 0; di[1] = 0;
	  }else{
		nrnran123_State** pv = (nrnran123_State**)(&_p_rng);
		nrnran123_getids(*pv, di, di+1);
	  }
//printf("StochKv.mod %p: bbcore_write offset=%d %d %d\n", _p, *offset, d?di[0]:-1, d?di[1]:-1);
	}
	*offset += 2;
}
static void bbcore_read(double* x, int* d, int* xx, int* offset, _threadargsproto_) {
	assert(!_p_rng);
	uint32_t* di = ((uint32_t*)d) + *offset;
        if (di[0] != 0 || di[1] != 0)
        {
	  nrnran123_State** pv = (nrnran123_State**)(&_p_rng);
	  *pv = nrnran123_newstream(di[0], di[1]);
        }
//printf("StochKv.mod %p: bbcore_read offset=%d %d %d\n", _p, *offset, di[0], di[1]);
	*offset += 2;
}
 
double brand ( _threadargsprotocomma_ double _lP , double _lN ) {
   double _lbrand;
 
/*VERBATIM*/
        /*
        :Supports separate independent but reproducible streams for
        : each instance. However, the corresponding hoc Random
        : distribution MUST be set to Random.uniform(0,1)
        */

        // Should probably be optimized
        double value = 0.0;
        int i;
        for (i = 0; i < _lN; i++) {
           if (urand(_threadargs_) < _lP) {
              value = value + 1;
           }
        }
        return(value);

 _lbrand = value ;
   
return _lbrand;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_brand(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  brand ( _p, _ppvar, _thread, _nt, *getarg(1) , *getarg(2) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
/*VERBATIM*/
#define        PI 3.141592654
#define        r_ia     16807
#define        r_im     2147483647
#define        r_am     (1.0/r_im)
#define        r_iq     127773
#define        r_ir     2836
#define        r_ntab   32
#define        r_ndiv   (1+(r_im-1)/r_ntab)
#define        r_eps    1.2e-7
#define        r_rnmx   (1.0-r_eps)
 
/*VERBATIM*/
/* ---------------------------------------------------------------- */
/* gammln - compute natural log of gamma function of xx */
static double
gammln(double xx)
{
    double x,tmp,ser;
    static double cof[6]={76.18009173,-86.50532033,24.01409822,
        -1.231739516,0.120858003e-2,-0.536382e-5};
    int j;
    x=xx-1.0;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.0;
    for (j=0;j<=5;j++) {
        x += 1.0;
        ser += cof[j]/x;
    }
    return -tmp+log(2.50662827465*ser);
}
 
double BnlDev ( _threadargsprotocomma_ double _lppr , double _lnnr ) {
   double _lBnlDev;
 
/*VERBATIM*/
        int j;
        double am,em,g,angle,p,bnl,sq,bt,y;
        double pc,plog,pclog,en,oldg;
        
        /* prepare to always ignore errors within this routine */
         
        p=(_lppr <= 0.5 ? _lppr : 1.0-_lppr);
        am=_lnnr*p;
        if (_lnnr < 25) {
            bnl=0.0;
            for (j=1;j<=_lnnr;j++)
                if (urand(_threadargs_) < p) bnl += 1.0;
        }
        else if (am < 1.0) {
            g=exp(-am);
            bt=1.0;
            for (j=0;j<=_lnnr;j++) {
                bt *= urand(_threadargs_);
                if (bt < g) break;
            }
            bnl=(j <= _lnnr ? j : _lnnr);
        }
        else {
            {
                en=_lnnr;
                oldg=gammln(en+1.0);
            }
            {
                pc=1.0-p;
                plog=log(p);
                pclog=log(pc);
            }
            sq=sqrt(2.0*am*pc);
            do {
                do {
                    angle=PI*urand(_threadargs_);
                        angle=PI*urand(_threadargs_);
                    y=tan(angle);
                    em=sq*y+am;
                } while (em < 0.0 || em >= (en+1.0));
                em=floor(em);
                    bt=1.2*sq*(1.0+y*y)*exp(oldg-gammln(em+1.0) - 
                    gammln(en-em+1.0)+em*plog+(en-em)*pclog);
            } while (urand(_threadargs_) > bt);
            bnl=em;
        }
        if (p != _lppr) bnl=_lnnr-bnl;
        
        /* recover error if changed during this routine, thus ignoring
            any errors during this routine */
       
        
        return bnl;
        
 _lBnlDev = bnl ;
   
return _lBnlDev;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_BnlDev(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  BnlDev ( _p, _ppvar, _thread, _nt, *getarg(1) , *getarg(2) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 static void _update_ion_pointer(Datum* _ppvar) {
 }

static void initmodel(double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt) {
  int _i; double _save;{
  n = n0;
 {
   
/*VERBATIM*/
	if (_p_rng) {
	      nrnran123_setseq((nrnran123_State*)_p_rng, 0, 0);
	}
	if (cvode_active_ && !deterministic) {
		hoc_execerror("StochKv with deterministic=0", "cannot be used with cvode");
	}
 eta = gkbar / gamma ;
   trates ( _threadargscomma_ v ) ;
   n = ninf ;
   scale_dens = gamma / area ;
   N = floor ( eta * area + 0.5 ) ;
   N1 = n * N ;
   if (  ! deterministic ) {
     N1 = floor ( N1 + 0.5 ) ;
     }
   N0 = N - N1 ;
   n0_n1 = 0.0 ;
   n1_n0 = 0.0 ;
   }
 
}
}

static void nrn_init(_NrnThread* _nt, _Memb_list* _ml, int _type){
double* _p; Datum* _ppvar; ThreadDatum* _thread;
double _v; int* _ni; int _iml, _cntml;
#if CACHEVEC
    _ni = _ml->_nodeindices;
#endif
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;

#if 0
 _check_trates(_p, _ppvar, _thread, _nt);
#endif
    _v = VEC_V(_ni[_iml]);
 v = _v;
  ek = _ion_ek;
 initmodel(_p, _ppvar, _thread, _nt);
 }
}

static double _nrn_current(double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt, double _v){double _current=0.;v=_v;{ {
   gk = ( strap ( _threadargscomma_ N1 ) * scale_dens * tadj ) ;
   ik = 1e-4 * gk * ( v - ek ) ;
   }
 _current += ik;

} return _current;
}

static void nrn_cur(_NrnThread* _nt, _Memb_list* _ml, int _type) {
double* _p; Datum* _ppvar; ThreadDatum* _thread;
int* _ni; double _rhs, _v; int _iml, _cntml;
#if CACHEVEC
    _ni = _ml->_nodeindices;
#endif
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
    _v = VEC_V(_ni[_iml]);
  ek = _ion_ek;
 _g = _nrn_current(_p, _ppvar, _thread, _nt, _v + .001);
 	{ double _dik;
  _dik = ik;
 _rhs = _nrn_current(_p, _ppvar, _thread, _nt, _v);
  _ion_dikdv += (_dik - ik)/.001 ;
 	}
 _g = (_g - _rhs)/.001;
  _ion_ik += ik ;
	VEC_RHS(_ni[_iml]) -= _rhs;
 
}
 
}

static void nrn_jacob(_NrnThread* _nt, _Memb_list* _ml, int _type) {
double* _p; Datum* _ppvar; ThreadDatum* _thread;
int* _ni; int _iml, _cntml;
#if CACHEVEC
    _ni = _ml->_nodeindices;
#endif
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize;
	VEC_D(_ni[_iml]) += _g;
 
}
 
}

static void nrn_state(_NrnThread* _nt, _Memb_list* _ml, int _type) {
#ifdef _PROF_HPM 
HPM_Start("nrn_state_StochKv"); 
#endif 
double* _p; Datum* _ppvar; ThreadDatum* _thread;
double _v = 0.0; int* _ni; int _iml, _cntml;
#if CACHEVEC
    _ni = _ml->_nodeindices;
#endif
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
    _v = VEC_V(_ni[_iml]);
 v=_v;
{
  ek = _ion_ek;
 {   states(_p, _ppvar, _thread, _nt);
  } }}
#ifdef _PROF_HPM 
HPM_Stop("nrn_state_StochKv"); 
#endif 

}

static void terminal(){}

static void _initlists(){
 double _x; double* _p = &_x;
 int _i; static int _first = 1;
  if (!_first) return;
 _slist1[0] = &(n) - _p;  _dlist1[0] = &(Dn) - _p;
   _t_ntau = makevector(200*sizeof(double));
   _t_ninf = makevector(200*sizeof(double));
   _t_a = makevector(200*sizeof(double));
   _t_b = makevector(200*sizeof(double));
   _t_tadj = makevector(200*sizeof(double));
_first = 0;
}

#if defined(__cplusplus)
} /* extern "C" */
#endif
