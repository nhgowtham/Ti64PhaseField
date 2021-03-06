#include "classes.h"

extern int input , output ;

extern double M0, R, T, cr, G_normalize, dt, lc, Dalal, Dvv, Vm, w_norm, epsi_norm, W_prefac, epsi_prefac, D, L_norm, v_alpha, heat, T_trans, L_orig, L, W_Al, W_V, W_Ti, kappa_c, fs ;

extern double G_Al_alpha, G_Al_beta, G_V_alpha, G_V_beta, G_Ti_alpha, G_Ti_beta ;
   
#define L0_BCC_Al_Ti_V (32045.963)
#define L1_BCC_Al_Ti_V (-113926.0 + 40*T) 
#define L2_BCC_Al_Ti_V (75972.5 - 150*T) 
#define L0_HCP_Al_Ti_V 0.0 
#define L1_HCP_Al_Ti_V (-206074.0 - 40*T) 
#define L2_HCP_Al_Ti_V 0 
#define L0_BCC_Ti_V (10500-1.5*T) 
#define L1_BCC_Ti_V 2025.39 
#define L2_BCC_Ti_V 0.0 
#define L0_HCP_Ti_V 20000.0
#define L1_HCP_Ti_V 0.0
#define L2_HCP_Ti_V 0.0
#define L0_BCC_Al_Ti (-128500.0+39*T+2520)
#define L1_BCC_Al_Ti 4890.0 
#define L2_BCC_Al_Ti 400.0 
#define L0_HCP_Al_Ti (-137500.0+39*T+2950)
#define L1_HCP_Al_Ti (-3475.0 + 0.825*T) 
#define L2_HCP_Al_Ti -7756.0 
#define L0_HCP_Al_V (-95000.0+20*T)
#define L1_HCP_Al_V 0.0
#define L2_HCP_Al_V 0.0
#define L0_BCC_Al_V (-95000.0+20*T)
#define L1_BCC_Al_V 6645.0 
#define L2_BCC_Al_V -68596.0 


#define Al_alpha_a_1 -2495.15 
#define Al_alpha_b_1 135.29 
#define Al_alpha_c_1 -24.37 
#define Al_alpha_d_1 -0.00188 

#define Al_alpha_a_2 -5795.24 
#define Al_alpha_b_2 221.25 
#define Al_alpha_c_2 -38.58 
#define Al_alpha_d_2 -0.01853 

#define Al_alpha_a_3 -5797.36 
#define Al_alpha_b_3 186.88 
#define Al_alpha_c_3 -31.75 
#define Al_alpha_d_3 0.0 

#define Al_beta_a_1 2106.85 ;
#define Al_beta_b_1 132.28 ;
#define Al_beta_c_1 -24.37 ;
#define Al_beta_d_1 -0.00188 ;

#define Al_beta_a_2 -1193.24 ;
#define Al_beta_b_2 218.24 ;
#define Al_beta_c_2 -38.58 ;
#define Al_beta_d_2 -0.01853 ;

#define Al_beta_a_3 -1195.36 
#define Al_beta_b_3 183.87 
#define Al_beta_c_3 -31.75 
#define Al_beta_d_3 0.0 

#define Ti_alpha_a_1 -8059.92 ;
#define Ti_alpha_b_1 133.62 ;
#define Ti_alpha_c_1 -23.99 ;
#define Ti_alpha_d_1 -0.00477 ;

#define Ti_alpha_a_2 -7811.82 
#define Ti_alpha_b_2 132.98 
#define Ti_alpha_c_2 -23.98 
#define Ti_alpha_d_2 -0.0042 

#define Ti_alpha_a_3 908.84 ;  
#define Ti_alpha_b_3 66.98 ;
#define Ti_alpha_c_3 -14.95 ;
#define Ti_alpha_d_3 -0.00815 ;

#define Ti_alpha_a_4 -124526.79 ;  
#define Ti_alpha_b_4 638.81 ;
#define Ti_alpha_c_4 -87.22 ;
#define Ti_alpha_d_4 -0.00821 ;

#define Ti_beta_a_1 -1272.06 
#define Ti_beta_b_1 134.71 
#define Ti_beta_c_1 -25.58 
#define Ti_beta_d_1 -0.00066 

#define Ti_beta_a_2 6667.39 ;
#define Ti_beta_b_2 105.37 ;
#define Ti_beta_c_2 -22.37 ;
#define Ti_beta_d_2 0.00122 ;

#define Ti_beta_a_3 26483.26 ;
#define Ti_beta_b_3 -182.43 ;
#define Ti_beta_c_3 19.09 ;
#define Ti_beta_d_3 -22.01 ;


#define V_alpha_a_1 -3930.43 ;
#define V_alpha_b_1 135.74 ; 
#define V_alpha_c_1 -24.13 ;
#define V_alpha_d_1 -0.0031 ;

#define V_alpha_a_2 -3967.84 
#define V_alpha_b_2 145.69 
#define V_alpha_c_2 -25.90 
#define V_alpha_d_2 0.000063 

#define V_alpha_a_3 -37689.86 ;
#define V_alpha_b_3 323.54 ;
#define V_alpha_c_3 -47.43 ;
#define V_alpha_d_3 0.0 ;

#define V_beta_a_1 -7930.43 ;
#define V_beta_b_1 133.35 ;
#define V_beta_c_1 -24.13 ;
#define V_beta_d_1 -0.0031 ;

#define V_beta_a_2 -7967.84 
#define V_beta_b_2 143.29 
#define V_beta_c_2 -25.90 
#define V_beta_d_2 0.000063 

#define V_beta_a_3 -41689.87 ;
#define V_beta_b_3 321.14 ;
#define V_beta_c_3 -47.43 ;
#define V_beta_d_3 0.0 ;

#define q_alpha_al_al -79800 + R*T*log(2.38e-05)
#define q_alpha_al_v -258550 + R*T*log(4.36e-02)
#define q_alpha_al_ti -193200 + R*T*log(1.0e-08)
#define q_alpha_v_al -79800 + R*T*log(2.38e-05)
#define q_alpha_v_v -258550 + R*T*log(4.36e-02)
#define q_alpha_v_ti -251490 -56*T

#define a_alpha_al_al_ti -491950
#define a_alpha_al_ti_v 587700
#define a_alpha_al_al_v 0.0
#define a_alpha_v_al_ti -529600
#define a_alpha_v_al_v 0.0
#define a_alpha_v_ti_v 0.0


#define q_beta_al_al -215000 -80.2*T
#define q_beta_al_v -268000 -97.2*T
#define q_beta_al_ti R*T*log(5.19e-10*exp(-96000/(R*T)))
#define q_beta_v_al -325008.12 -73.99*T
#define q_beta_v_v -325008.12 -73.99*T
#define q_beta_v_ti -179392.69 - 106.68*T

#define a_beta_al_al_ti -499946.15 + 333.87*T
#define a1_beta_al_al_ti -407271.50 + 286.27*T  
#define a_beta_al_al_v -751405.09
#define a_beta_al_ti_v 0.0
#define a_beta_v_al_v -1300707.37
#define a_beta_v_ti_v +159597.07 - 48.62*T
#define a1_beta_v_ti_v -10551.39
#define a_beta_v_al_ti 0.0
                
extern double epsi[12][3][3], c[6][6], sigma00[12][6] ;

extern sfts eigen_alpha[12];

extern double PI;







