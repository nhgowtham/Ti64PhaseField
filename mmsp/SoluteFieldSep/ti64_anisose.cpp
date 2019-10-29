#include<iomanip>
#include<vector>
#include<cmath>
#include<ctime>
#include<cstdio>
#include<cstdlib>
#include<cassert>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<deque>
#include<vector>
#include"MMSP.hpp"

double epsi[12][3][3] ;
#include "GradECoeff.hpp"

class sfts
{
	
	public:
	
	double e1, e2, e3, e4, e5, e6 ;
	
	sfts()
	{
		e1 = 0.0 ;
		e2 = 0.0 ;
		e3 = 0.0 ;
		e4 = 0.0 ;
		e5 = 0.0 ;
		e6 = 0.0 ;
	}
};


using namespace std;

typedef float phi_type; 											
typedef MMSP::sparse<phi_type> store_type; 	

double R = 8.314 ;
double T = 1023 ;
double T_orig = 1023;
double cr = 10.0;

double L_orig = 0.239 ;
double G_normalize = R*T ; 



double lold = 9.677*pow(10, -8) ;
double lc = 9.677*pow(10,-8) ;
double dx_nd = lold/lc ;


double tc = 4.4 ; //seconds
double dt_old = 0.44 ; //seconds
double dt = dt_old/tc; 

double Dalal = 0.0001 ; 
double Dalv = -0.00008 ;
double Dvv = 0.0001 ;
double Dval = -0.00008 ;
void thermo_auxillary_terms(MMSP::vector<store_type> gradient, MMSP::vector<store_type> gradientsq, double c_Al, double c_V) ;
double dGAlpha_dAl,	dGBeta_dAl, dGAlpha_dV, dGBeta_dV, del_dGAlpha_dAl, del_dGBeta_dAl, del_dGAlpha_dV, del_dGBeta_dV, delsq_dGAlpha_dAl, delsq_dGAlpha_dV, delsq_dGBeta_dAl, delsq_dGBeta_dV ;
double G_Al_alpha, G_Ti_alpha, G_V_alpha, G_Al_beta, G_Ti_beta, G_V_beta , W_Al, W_V, W_Ti ; 
#include "Thermo.hpp"

double Vm = 0.00001;

double w_norm = (R*T)/Vm ;
double epsi_norm = (R*T*pow(lc,2))/Vm;




int variants = 10 ;
const int dim = 3;
const int nx = 60;
const int ny = 60;
const int nz = 60;
MMSP::grid<dim, store_type> gradsqcal_grid(variants, 0, nx, 0, ny, 0, nz) ;
MMSP::grid<dim, store_type> gradsqcv_grid(variants, 0, nx, 0, ny, 0, nz) ;
const int node_total = nx*ny*nz ; // (int)nodes(grid) ; 
double G_Alpha[node_total], G_Beta[node_total], W[node_total] ; 
double hphi[node_total], hphiprime[node_total], hphidoubleprime[node_total];
double gphi[node_total], gphiprime[node_total], gphidoubleprime[node_total];

MMSP::grid<dim, store_type> grid(variants, 0, nx, 0, ny, 0, nz) ;
MMSP::grid<dim, store_type> gradgrid(variants, 0, nx, 0, ny, 0, nz) ;

const double Lx = g1(grid,0) - g0(grid,0) ;
const double Ly = g1(grid,1) - g0(grid,1) ;
const double Lz = g1(grid,2) - g0(grid,2) ;

const double dx = MMSP::dx(grid, 0) ;

double W_prefac = 6.095 ;


void initialize()
{
	
	W_Al = 0.1*1.0 ; //6.095 ; 
	W_V =  0.1*1.0 ; //3.03 ;
	W_Ti = 0.1*1.5 ; //10.05 ;
	
	G_Al_alpha = (Al_alpha_a_3 + Al_alpha_b_3*T + Al_alpha_c_3*T*log(T) + Al_alpha_d_3*T*T) ;
	G_Al_beta = (Al_beta_a_3 + Al_beta_b_3*T + Al_beta_c_3*T*log(T) + Al_beta_d_3*T*T) ;
	
	G_Ti_alpha = (Ti_alpha_a_2 + Ti_alpha_b_2*T + Ti_alpha_c_2*T*log(T) + Ti_alpha_d_2*T*T) ;
	G_Ti_beta = (Ti_beta_a_1 + Ti_beta_b_1*T + Ti_beta_c_1*T*log(T) + Ti_beta_d_1*T*T) ;
	
	G_V_alpha = (V_alpha_a_2 + V_alpha_b_2*T + V_alpha_c_2*T*log(T) + V_alpha_d_2*T*T) ; //(21.96 - T/50)
	G_V_beta = (V_beta_a_2 + V_beta_b_2*T + V_beta_c_2*T*log(T) + V_beta_d_2*T*T) ;

	for(int n = 0; n < nodes(grid); n++)
	{
		MMSP::vector<int> x = position(grid, n);
		
		set(gradsqcal_grid(n), 1) = 0.0 ;
		set(gradsqcv_grid(n), 1) = 0.0 ;
		
		store_type newGrain;
		
		
			set(newGrain, 15) = 1.0;
			set(newGrain, 1) = 0.0;
			set(newGrain, 2) = 0.0;
			set(newGrain, 3) = 0.0;
			set(newGrain, 4) = 0.0;
			set(newGrain, 5) = 0.0;
			set(newGrain, 20) = 0.06;
			set(newGrain, 21) = 0.04 ;
			
		//vi ~/mmsp/include/MMSP.grid.cpp

		
		grid(n) = newGrain;		
		
		G_Alpha[n] = ((grid(n)[20]*G_Al_alpha + grid(n)[21]*G_V_alpha + (1-grid(n)[20]-grid(n)[21])*G_Ti_alpha +
					 R*T*(grid(n)[20]*log(grid(n)[20]) + grid(n)[21]*log(grid(n)[21]) + (1-grid(n)[20]-grid(n)[21])*log((1-grid(n)[20]-grid(n)[21]))) +
					 grid(n)[20]*grid(n)[21]*L0_HCP_Al_V + grid(n)[20]*(1-grid(n)[20]-grid(n)[21])*L0_HCP_Al_Ti + grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_HCP_Ti_V))/G_normalize ;
					 
		G_Beta[n] = (grid(n)[20]*G_Al_beta + grid(n)[21]*G_V_beta + (1-grid(n)[20]-grid(n)[21])*G_Ti_beta +
					 R*T*(grid(n)[20]*log(grid(n)[20]) + grid(n)[21]*log(grid(n)[21]) + (1-grid(n)[20]-grid(n)[21])*log((1-grid(n)[20]-grid(n)[21]))) +
					 grid(n)[20]*grid(n)[21]*L0_BCC_Al_V + grid(n)[20]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Al_Ti + grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Ti_V +
					 grid(n)[20]*grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Al_Ti_V)/G_normalize ;
					 
		
		W[n] = W_prefac*(grid(n)[20]*W_Al + grid(n)[21]*W_V + (1-grid(n)[20]-grid(n)[21])*W_Ti) ;
		
	}

}

int main()
{	
	//cout<<(6.8*pow(10,-6)/epsi_norm)<<endl;
	
	

	string file_name ;
	string file_name1 ;
	string file_name2 ;
	string file_name3 ;

	int steps = 70000 ;  	

	initialize();
	initialize_epsi();

	MMSP::b0(grid,0) = MMSP::Dirichlet;
	MMSP::b1(grid,0) = MMSP::Dirichlet;

	MMSP::b0(grid,1) = MMSP::Dirichlet;
	MMSP::b1(grid,1) = MMSP::Dirichlet;

	MMSP::b0(grid,2) = MMSP::Dirichlet;
	MMSP::b1(grid,2) = MMSP::Dirichlet;

	double lap_max ;

	for(int t = 0 ; t<steps ; t++)
	{
		//cout<<t<<endl;
		//T = T - dt*tc*cr ;

		MMSP::grid<dim, store_type > update(grid);
	
		MMSP::b0(update,0) = MMSP::Dirichlet;
		MMSP::b1(update,0) = MMSP::Dirichlet;

		MMSP::b0(update,1) = MMSP::Dirichlet;
		MMSP::b1(update,1) = MMSP::Dirichlet;

		MMSP::b0(update,2) = MMSP::Dirichlet;
		MMSP::b1(update,2) = MMSP::Dirichlet;
	
		if(t%100==0)
		{
			std::ostringstream fileNameStream("pf_") ;
			fileNameStream<<"pf_"<<t<<".txt";
			file_name = fileNameStream.str() ;
			ofstream myfile;
			myfile.open(file_name.c_str());
			for(int n = 0 ; n < nodes(grid) ; n++)    
			{
				MMSP::vector<int> s = position(grid,n) ;
				double sum = 0 ;
				for(int h = 0 ; h < length(grid(s)) ; h++)
				{
					int hindex = MMSP::index(grid(s),h) ;
					if(hindex <= 12)
					{
						sum+=grid(s)[hindex]*grid(s)[hindex] ;
					}
				}
			
				myfile<<s[0]<<","<<s[1]<<","<<s[2]<<","<<sum<<"\n" ;    
			
			}
	
			myfile.close();
			
			std::ostringstream fileNameStream1("pf3_") ;
			fileNameStream1<<"c_Al_"<<t<<".txt";
			file_name1 = fileNameStream1.str() ;
			ofstream myfile1;
			myfile1.open(file_name1.c_str());
			std::ostringstream fileNameStream2("c_V_") ;
			fileNameStream2<<"c_V_"<<t<<".txt";
			file_name2 = fileNameStream2.str() ;
			ofstream myfile2;
			myfile2.open(file_name2.c_str());
			for(int n = 0 ; n < nodes(grid) ; n++)    //for n < nodes(grid) ; 
			{
				MMSP::vector<int> s = position(grid,n) ;
				myfile1<<s[0]<<","<<s[1]<<","<<s[2]<<","<<grid(s)[20]<<"\n" ;
				myfile2<<s[0]<<","<<s[1]<<","<<s[2]<<","<<grid(s)[21]<<"\n" ;
			}
		
			std::ostringstream fileNameStream3 ;   //Check for a possible error here
			fileNameStream3<<"deltachem_"<<t<<".txt";
			file_name3 = fileNameStream3.str() ;
			ofstream myfile3;
			myfile3.open(file_name3.c_str());
		
		
		//-----------Calculation of chemical free energy driving force per volume for nucleation------------------//
		
			for(int n = 0 ; n < nodes(grid) ; n++)
			{
				MMSP::vector<int> s = position(grid,n) ;
				double g_alpha = ((grid(n)[20]*G_Al_alpha + grid(n)[21]*G_V_alpha + (1-grid(n)[20]-grid(n)[21])*G_Ti_alpha +
					 R*T*(grid(n)[20]*log(grid(n)[20]) + grid(n)[21]*log(grid(n)[21]) + (1-grid(n)[20]-grid(n)[21])*log((1-grid(n)[20]-grid(n)[21]))) +
					 grid(n)[20]*grid(n)[21]*L0_HCP_Al_V + grid(n)[20]*(1-grid(n)[20]-grid(n)[21])*L0_HCP_Al_Ti + grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_HCP_Ti_V))/G_normalize ;
					 
				double g_beta = (grid(n)[20]*G_Al_beta + grid(n)[21]*G_V_beta + (1-grid(n)[20]-grid(n)[21])*G_Ti_beta +
					 R*T*(grid(n)[20]*log(grid(n)[20]) + grid(n)[21]*log(grid(n)[21]) + (1-grid(n)[20]-grid(n)[21])*log((1-grid(n)[20]-grid(n)[21]))) +
					 grid(n)[20]*grid(n)[21]*L0_BCC_Al_V + grid(n)[20]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Al_Ti + grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Ti_V +
					 grid(n)[20]*grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Al_Ti_V)/G_normalize ;
					 myfile3<<s[0]<<","<<s[1]<<","<<s[2]<<","<<(g_beta-g_alpha)<<"\n" ;
			}
		
			myfile1.close();
			myfile2.close();
			myfile3.close();
		
		
		}
	
		double deltag = 0.1 ;
		double kappa1 = 0.02 ; 
		double kappa2 = 0.1 ; 
	
		int no_of_cells = (nx*ny*nz)/(3*3*3) ; 
		int cells_in_one_dir = nx/3 ; 
	
		if((t%100==0)&(t<300))   
		{
			MMSP::vector<int> x(3) ; 
			MMSP::vector<int> y(3) ; 
			MMSP::vector<int> z(3) ; 
			MMSP::vector<int> temp_pos(3); 
			for(int i = 0 ; i < cells_in_one_dir ; i++)
			{
				for(int j = 0 ; j < cells_in_one_dir ; j++)
				{
					for(int k = 0 ; k < cells_in_one_dir ; k++)
					{
						int flag = 0 ;
				
						x[0] = i*3 ; x[1] = i*3+1 ; x[2] = i*3+2 ; 
				
						y[0] = j*3 ; y[1] = j*3+1 ; y[2] = j*3+2 ; 
				
						z[0] = k*3 ; z[1] = k*3+1 ; z[2] = k*3+2 ; 
						for(int p = 0; p < 3; p++)
						{
							for(int q = 0 ; q < 3 ; q++)
							{
								for(int r = 0 ; r < 3 ; r++)
								{							
									temp_pos[0] = x[p] ; temp_pos[1] = y[q] ; temp_pos[2] = z[r] ;
									flag+= grid(temp_pos)[15] ;
								}
							}
						}
				
						if(flag > 0)
						{					
							double jstar = kappa1*exp(-kappa2*deltag);  
							double prob = 1 - exp(-jstar) ;
							double random_no = (double)(rand() % 1000) ;
							double decision_var = random_no/1000.0 ;
							if(prob > decision_var)
							{
								store_type newGrain;
								int random_index = (int)(rand() % 5) + 1 ;
								cout<<"Nucleating index :"<<random_index<<endl;
								/*for(int m = 0 ; m < nodes(grid) ; m++)
								{
									if(length(grid(m))==1) set(grid(m), random_index) = 0.0 ;
									else if(length(grid(m))> 1)
									{
										int temp_check = 0 ; 
										for(int h = 0 ; h < length(grid(m)) ; h++)
										{
											if(MMSP::index(grid(m), h) == random_index)
											{
												temp_check = 1 ;
												break;
											}
										}
										if(temp_check == 0) set(grid(m), random_index) = 0.0; 
									}
								}*/
								for(int p = 0; p < 3; p++)
								{
									for(int q = 0 ; q < 3 ; q++)
									{
										for(int r = 0 ; r < 3 ; r++)
										{
											temp_pos[0] = x[p] ; temp_pos[1] = y[q] ; temp_pos[2] = z[r] ;
									
											if((pow((temp_pos[0]*MMSP::dx(grid,0)-x[2]*MMSP::dx(grid,0)),2) + pow((temp_pos[1]*MMSP::dx(grid,1)-y[2]*MMSP::dx(grid,0)),2) + pow((temp_pos[2]*MMSP::dx(grid,2)-z[2]*MMSP::dx(grid,0)),2)) < 3*MMSP::dx(grid,0)*MMSP::dx(grid,0))
											{	
												set(grid(temp_pos), 15) = 0.0 ;   
												set(grid(temp_pos), random_index) = 1.0; 	
											}
										}
									}
								}
						
							}
						}
					}
				}
			}
		}
		
		for(int n = 0 ; n<nodes(grid) ; n++)
		{
			MMSP::vector<int> x = position(grid, n); 
			MMSP::vector<store_type> gradientsqtemp = gradsq(grid, x) ;
			set(gradsqcal_grid(n),1) = gradientsqtemp[0][20] + gradientsqtemp[1][20] + gradientsqtemp[2][20] ;
			set(gradsqcv_grid(n), 1) = gradientsqtemp[0][21] + gradientsqtemp[1][21] + gradientsqtemp[2][21] ;
		}
	
	
		
	
	
		for(int n = 0 ; n < nodes(grid) ; n++)
		{
			MMSP::vector<int> x = position(grid, n); 
			G_Alpha[n] = ((grid(n)[20]*G_Al_alpha + grid(n)[21]*G_V_alpha + (1-grid(n)[20]-grid(n)[21])*G_Ti_alpha +
					 R*T*(grid(n)[20]*log(grid(n)[20]) + grid(n)[21]*log(grid(n)[21]) + (1-grid(n)[20]-grid(n)[21])*log((1-grid(n)[20]-grid(n)[21]))) +
					 grid(n)[20]*grid(n)[21]*L0_HCP_Al_V + grid(n)[20]*(1-grid(n)[20]-grid(n)[21])*L0_HCP_Al_Ti + grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_HCP_Ti_V))/G_normalize ;
					 
			G_Beta[n] = (grid(n)[20]*G_Al_beta + grid(n)[21]*G_V_beta + (1-grid(n)[20]-grid(n)[21])*G_Ti_beta +
					 R*T*(grid(n)[20]*log(grid(n)[20]) + grid(n)[21]*log(grid(n)[21]) + (1-grid(n)[20]-grid(n)[21])*log((1-grid(n)[20]-grid(n)[21]))) +
					 grid(n)[20]*grid(n)[21]*L0_BCC_Al_V + grid(n)[20]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Al_Ti + grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Ti_V +
					 grid(n)[20]*grid(n)[21]*(1-grid(n)[20]-grid(n)[21])*L0_BCC_Al_Ti_V)/G_normalize ;
					 
			W[n] = W_prefac*(grid(n)[20]*W_Al + grid(n)[21]*W_V + (1-grid(n)[20]-grid(n)[21])*W_Ti) ;
		
		
			MMSP::vector<store_type> gradient = grad(grid, x) ;
			MMSP::vector<store_type> gradientsq = gradsq(grid, x) ;
		
		
		
		
		
			thermo_auxillary_terms(gradient, gradientsq, grid(n)[20], grid(n)[21]) ;

			MMSP::vector<store_type> gradcalp4 = gradsq(gradsqcal_grid, x) ;
			MMSP::vector<store_type> gradcvp4 = gradsq(gradsqcv_grid, x) ;
		
			double gradp4_cal = gradcalp4[0][1] + gradcalp4[1][1] + gradcalp4[2][1] ; //gradientsq[2][20] ;
			double gradp4_cv = gradcvp4[0][1] + gradcvp4[1][1] + gradcvp4[2][1] ;// gradientsq[2][21] ;
				
			double phi_grad = 0 ;
			double phi_gradientsq = 0 ;
			for(int h = 0 ; h < length(grid(x)) ; h++)
			{
				int hindex = MMSP::index(grid(x),h) ;
				if((hindex <= 12)&&(grid(n)[hindex]>1e-02))
				{
				
					for(int i = 0 ; i < dim ; i++)
					{
						phi_grad+= gradient[i][hindex] ; 
						phi_gradientsq+= gradientsq[i][hindex] ; 
					}
				}
			}
		
			hphidoubleprime[n] = 0.0 ; 
			hphi[n] = 0.0 ;
			hphiprime[n] = 0.0 ;
		
			gphidoubleprime[n] = 0.0 ; 
			gphiprime[n] = 0.0 ;
			gphi[n] = 0.0 ;
		
			for(int h = 0 ; h < length(grid(x)) ; h++)
			{
				int hindex = MMSP::index(grid(x),h) ;
				if((hindex <= 12))
				{
					hphidoubleprime[n] +=  2*pow(grid(n)[hindex],1) - 3*pow(grid(n)[hindex],2) ; 
					hphi[n] += pow(grid(n)[hindex],3)/3 - pow(grid(n)[hindex],4)/4 ;
					hphiprime[n] += pow(grid(n)[hindex],2) - pow(grid(n)[hindex],3) ;
				
					gphidoubleprime[n] += (2*(6*pow(grid(n)[hindex],2) - 6*grid(n)[hindex] + 1)) ; 
					gphiprime[n] += (2*grid(n)[hindex]*(1-grid(n)[hindex])*(1-2*grid(n)[hindex])) ;
					gphi[n] += pow(grid(n)[hindex],2)*pow((1-grid(n)[hindex]),2) ;
				}
			}
													 
			double c_al_rhs = 2*(del_dGAlpha_dAl - del_dGBeta_dAl)*hphiprime[n]*phi_grad + delsq_dGAlpha_dAl*hphi[n] + delsq_dGBeta_dAl*(1-hphi[n]) + 
						  (dGAlpha_dAl - dGBeta_dAl)*(hphidoubleprime[n]*pow(phi_grad,2) + hphiprime[n]*phi_gradientsq) + (W_Al - W_Ti)*(gphiprime[n]*phi_gradientsq + gphidoubleprime[n]*phi_grad) ;
			double c_v_rhs = 2*(del_dGAlpha_dV - del_dGBeta_dV)*hphiprime[n]*phi_grad + delsq_dGAlpha_dV*hphi[n] + delsq_dGBeta_dV*(1-hphi[n]) + 
						  (dGAlpha_dV - dGBeta_dV)*(hphidoubleprime[n]*pow(phi_grad,2) + hphiprime[n]*phi_gradientsq) + (W_V - W_Ti)*(gphiprime[n]*phi_gradientsq + gphidoubleprime[n]*phi_grad)  ;
						  

			set(update(n), 20) = grid(n)[20] + dt*(Dalal*c_al_rhs + Dalv*c_v_rhs - 0.076*gradp4_cal - 0.00076*gradp4_cv) ; //laplacian(grid,n)[20]) ;
			set(update(n), 21) = grid(n)[21] + dt*(Dvv*c_v_rhs + Dval*c_al_rhs -  0.076*gradp4_cv - 0.00076*gradp4_cal) ; //laplacian(grid,n)[21]) ;
		
		
		
			for (int j = 0; j < length(grid(n)); j++) 
			{
				int jindex = MMSP::index(grid(n), j);
				if(jindex<=12)
				{
					set(gradgrid(n),(3*(jindex-1)+1)) = gradient[0][jindex];
					set(gradgrid(n),(3*(jindex-1)+2)) = gradient[1][jindex];
					set(gradgrid(n),(3*(jindex-1)+3)) = gradient[2][jindex];
				}
			}
		
		

			double lap_aniso[13];
				
			MMSP::sparse<phi_type> dFdp;
			
			phi_type phi_sq = 0.0; 	
			phi_type phi_sum = 0.0 ;						
			phi_type dFall = 0.0;
			phi_type dFall_no = 0 ;
					
														
			for (int j = 0; j < length(grid(n)); j++) 
			{
				int jindex = MMSP::index(grid(n), j);
				if(jindex<=12)
				{	
					phi_sq += grid(n)[jindex]*grid(n)[jindex];
					phi_sum += grid(n)[jindex] ;
				}
			}
				
			phi_type phi_beta = 1 - phi_sum ; 
				
			MMSP::sparse<phi_type> lap = laplacian(grid, n); 
				
			int nz = 0;
			
			for (int j = 0; j < length(grid(n)); j++)   
			{
				int jindex = MMSP::index(grid(n), j);  		
					
				MMSP::vector<int> x = position(grid, n);
				MMSP::vector<double> gradients(3) ;
					
				W[n] = 0.01432 ; //1144*pow(10,5)/w_norm ; //
					
				MMSP::vector<store_type> gradgrad = grad(gradgrid, x) ;
				if(jindex <= 12)
				{
					lap_aniso[jindex] = 0.0 ;
					for(int p = 0 ; p < dim ; p++)
					{
						for(int q = 0 ; q < dim ; q++)
						{
							lap_aniso[jindex]+= (6.8*pow(10,-6)/epsi_norm)*(gradient[p][jindex])*epsi[jindex-1][p][q]*(gradient[q][jindex]);   //
						}
					}
					
					double interaction_energy = 0 ;
					interaction_energy+= W[n]*grid(n)[jindex]*pow(phi_beta, 2);
					interaction_energy-= W[n]*pow(grid(n)[jindex],2)*(phi_beta);
					
					int check = 0;
					for(int k = 0 ; k < length(grid(n)); k++)
					{
						int tempindex = MMSP::index(grid(n), k); 
						if((tempindex!=jindex)&&(tempindex <=12))
						{
							interaction_energy+= W[n]*pow(grid(n)[tempindex],2)*grid(n)[jindex];
							interaction_energy-= W[n]*pow(grid(n)[tempindex],2)*(phi_beta); 
						}
					}
					
					double coeff ;
					if(lap[jindex]==0) coeff = 0.0 ;
					else coeff = coeff = ((lap[jindex])/(abs(lap[jindex]))) ;
					//set(dFdp, jindex) = (-2.0)*(pow(grid(n)[jindex],2) - pow(grid(n)[jindex],3)) + (-1.1)*(-pow(grid(n)[jindex],2) + pow(grid(n)[jindex],3))   + interaction_energy - coeff*lap_aniso[jindex] ; 
					//set(dFdp, jindex) = pow(grid(n)[jindex],3) - pow(grid(n)[jindex],2) + interaction_energy - coeff*lap_aniso[jindex] ; 
					set(dFdp, jindex) = (grid(n)[jindex] * (phi_sq - grid(n)[jindex])) + interaction_energy  - coeff*(lap_aniso[jindex]); 
					
					dFall += dFdp[jindex];	
					dFall_no+=1;
				}
										
			}
			
			double L = L_orig*(T/T_orig) ;

			for (int h = 0; h < length(grid(n)); h++) 
			{	
					
				int hindex = MMSP::index(grid(n), h);   
				if(hindex<=12)
				{    				
					
				store_type dpdt;
				if(dFall_no > 1) set(dpdt, hindex) = -L * ((dFall_no+1)*dFdp[hindex] - dFall);
				else set(dpdt, hindex) = -L * ((dFall_no+1)*dFdp[hindex] - dFall);
				phi_type value = grid(n)[hindex] + dt * dpdt[hindex];	
				set(update(n), hindex) = value;	
				}
			}
			
			set(update(n), 15) = grid(n)[15] ;
			
		}

		swap(grid, update);	

}



return 0 ;

}






