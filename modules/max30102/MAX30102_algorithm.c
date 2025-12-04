#include "MAX30102_algorithm.h"
#include "MAX30102.h"
#include "software_IIC.h"
#include "Delay.h"
#define XPI            	(3.1415926535897932384626433832795)
#define XENTRY        	(100)
#define XINCL        	(XPI/2/XENTRY)
#define PI 				3.1415926535897932384626433832795028841971               //定义圆周率值

/*Global data space ----------------------------------------------------------*/
//正弦值对应表
static const double XSinTbl[] = {
			0.00000000000000000  , 0.015707317311820675 , 0.031410759078128292 , 0.047106450709642665 , 0.062790519529313374 ,
			0.078459095727844944 , 0.094108313318514325 , 0.10973431109104528  , 0.12533323356430426  , 0.14090123193758267  ,
			0.15643446504023087  , 0.17192910027940955  , 0.18738131458572463  , 0.20278729535651249  , 0.21814324139654256  ,
			0.23344536385590542  , 0.24868988716485479  , 0.26387304996537292  , 0.27899110603922928  , 0.29404032523230400  ,
			0.30901699437494740  , 0.32391741819814940  , 0.33873792024529142  , 0.35347484377925714  , 0.36812455268467797  ,
			0.38268343236508978  , 0.39714789063478062  , 0.41151435860510882  , 0.42577929156507272  , 0.43993916985591514  ,
			0.45399049973954680  , 0.46792981426057340  , 0.48175367410171532  , 0.49545866843240760  , 0.50904141575037132  ,
			0.52249856471594880  , 0.53582679497899666  , 0.54902281799813180  , 0.56208337785213058  , 0.57500525204327857  ,
			0.58778525229247314  , 0.60042022532588402  , 0.61290705365297649  , 0.62524265633570519  , 0.63742398974868975  ,
			0.64944804833018377  , 0.66131186532365183  , 0.67301251350977331  , 0.68454710592868873  , 0.69591279659231442  ,
			0.70710678118654757  , 0.71812629776318881  , 0.72896862742141155  , 0.73963109497860968  , 0.75011106963045959  ,
			0.76040596560003104  , 0.77051324277578925  , 0.78043040733832969  , 0.79015501237569041  , 0.79968465848709058  ,
			0.80901699437494745  , 0.81814971742502351  , 0.82708057427456183  , 0.83580736136827027  , 0.84432792550201508  ,
			0.85264016435409218  , 0.86074202700394364  , 0.86863151443819120  , 0.87630668004386369  , 0.88376563008869347  ,
			0.89100652418836779  , 0.89802757576061565  , 0.90482705246601958  , 0.91140327663544529  , 0.91775462568398114  ,
			0.92387953251128674  , 0.92977648588825146  , 0.93544403082986738  , 0.94088076895422557  , 0.94608535882754530  ,
			0.95105651629515353  , 0.95579301479833012  , 0.96029368567694307  , 0.96455741845779808  , 0.96858316112863108  ,
			0.97236992039767667  , 0.97591676193874743  , 0.97922281062176575  , 0.98228725072868872  , 0.98510932615477398  ,
			0.98768834059513777  , 0.99002365771655754  , 0.99211470131447788  , 0.99396095545517971  , 0.99556196460308000  ,
			0.99691733373312796  , 0.99802672842827156  , 0.99888987496197001  , 0.99950656036573160  , 0.99987663248166059  ,
			1.00000000000000000  };
			
//向下取整
double my_floor(double x)
{
   double y=x;
    if( (*( ( (int *) &y)+1) & 0x80000000)  != 0) //或者if(x<0)
        return (float)((int)x)-1;
    else
        return (float)((int)x);
}

//求余运算
double my_fmod(double x, double y)
{
   double temp, ret;
  
   if (y == 0.0)
      return 0.0;
   temp = my_floor(x/y);
   ret = x - temp * y;
   if ((x < 0.0) != (y < 0.0))
      ret = ret - y;
   return ret;
}

//正弦函数
double XSin( double x )
{
    int s = 0 , n;
    double dx , sx , cx;
    if( x < 0 )
        s = 1 , x = -x;
    x = my_fmod( x , 2 * XPI );
    if( x > XPI )
        s = !s , x -= XPI;
    if( x > XPI / 2 )
        x = XPI - x;
    n = (int)( x / XINCL );
    dx = x - n * XINCL;
    if( dx > XINCL / 2 )
        ++n , dx -= XINCL;
    sx = XSinTbl[n];
    cx = XSinTbl[XENTRY-n];
    x = sx + dx*cx - (dx*dx)*sx/2
        - (dx*dx*dx)*cx/6 
        + (dx*dx*dx*dx)*sx/24;
     
    return s ? -x : x;
}

//余弦函数 
double XCos( double x )
{
    return XSin( x + XPI/2 );
}

//开平方
int qsqrt(int a)
{
  uint32_t rem = 0, root = 0, divisor = 0;
  uint16_t i;
  for(i=0; i<16; i++)
  {
    root <<= 1;
    rem = ((rem << 2) + (a>>30));
    a <<= 2;
    divisor = (root << 1) + 1;
    if(divisor <= rem)
    {
      rem -= divisor;
      root++;
    }
  }
  return root;
}

/*********************************FFT*********************************
                         快速傅里叶变换C函数
函数简介：此函数是通用的快速傅里叶变换C语言函数，移植性强，以下部分不依
          赖硬件。此函数采用联合体的形式表示一个复数，输入为自然顺序的复
          数（输入实数是可令复数虚部为0），输出为经过FFT变换的自然顺序的
          复数
使用说明：使用此函数只需更改宏定义FFT_N的值即可实现点数的改变，FFT_N的
          应该为2的N次方，不满足此条件时应在后面补0
函数调用：FFT(s);
时    间：2010-2-20
版    本：Ver1.0
参考文献：     
**********************************************************************/

/*******************************************************************
函数原型：struct compx EE(struct compx b1,struct compx b2)  
函数功能：对两个复数进行乘法运算
输入参数：两个以联合体定义的复数a,b
输出参数：a和b的乘积，以联合体的形式输出
*******************************************************************/
struct compx EE(struct compx a,struct compx b)      
{
	 struct compx c;
	 c.real=a.real*b.real-a.imag*b.imag;
	 c.imag=a.real*b.imag+a.imag*b.real;
	 return(c);
}

/*****************************************************************
函数原型：void FFT(struct compx *xin,int N)
函数功能：对输入的复数组进行快速傅里叶变换（FFT）
输入参数：*xin复数结构体组的首地址指针，struct型
*****************************************************************/
void FFT(struct compx *xin)
{
	int f,m,nv2,nm1,i,k,l,j=0;
	struct compx u,w,t;

	nv2=FFT_N/2;                  //变址运算，即把自然顺序变成倒位序，采用雷德算法
	nm1=FFT_N-1;  
	for(i=0;i<nm1;i++)        
	{
		if(i<j)                    //如果i<j,即进行变址
		{
			t=xin[j];           
			xin[j]=xin[i];
			xin[i]=t;
		}
		k=nv2;                    //求j的下一个倒位序
		
		while(k<=j)               //如果k<=j,表示j的最高位为1   
		{           
			j=j-k;                 //把最高位变成0
			k=k/2;                 //k/2，比较次高位，依次类推，逐个比较，直到某个位为0
		}
		
		j=j+k;                   //把0改为1
	}
	 
	{  //FFT运算核，使用蝶形运算完成FFT运算
		int le,lei,ip;                           
		f=FFT_N;
		for(l=1;(f=f/2)!=1;l++)                  //计算l的值，即计算蝶形级数
			;
		for(m=1;m<=l;m++)                           // 控制蝶形结级数
		{                                           //m表示第m级蝶形，l为蝶形级总数l=log（2）N
			le=2<<(m-1);                            //le蝶形结距离，即第m级蝶形的蝶形结相距le点
			lei=le/2;                               //同一蝶形结中参加运算的两点的距离
			u.real=1.0;                             //u为蝶形结运算系数，初始值为1
			u.imag=0.0;
			w.real=XCos(PI/lei);                     //w为系数商，即当前系数与前一个系数的商
			w.imag=-XSin(PI/lei);
			for(j=0;j<=lei-1;j++)                   //控制计算不同种蝶形结，即计算系数不同的蝶形结
			{
				for(i=j;i<=FFT_N-1;i=i+le)            //控制同一蝶形结运算，即计算系数相同蝶形结
				{
					ip=i+lei;                           //i，ip分别表示参加蝶形运算的两个节点
					t=EE(xin[ip],u);                    //蝶形运算，详见公式
					xin[ip].real=xin[i].real-t.real;
					xin[ip].imag=xin[i].imag-t.imag;
					xin[i].real=xin[i].real+t.real;
					xin[i].imag=xin[i].imag+t.imag;
				}
				u=EE(u,w);                           //改变系数，进行下一个蝶形运算
			}
		}
	}
}

//读取峰值
int find_max_num_index(struct compx *data,int count)
{
	int i=START_INDEX;
	int max_num_index = i;
	//struct compx temp=data[i];
	float temp = data[i].real;
	for(i=START_INDEX;i<count;i++)
	{
		if(temp < data[i].real)
		{
			temp = data[i].real;
			max_num_index = i;
		}
	}
	return max_num_index; 
}


//直流滤波器
int dc_filter(int input,DC_FilterData * df) 
{

	float new_w  = input + df->w * df->a;
	int16_t result = 5*(new_w - df->w);
	df->w = new_w;
	
	return result;
}


int bw_filter(int input,BW_FilterData * bw) {
    bw->v0 = bw->v1;
    
   // v1 = (3.04687470e-2 * input) + (0.9390625058 * v0);
    bw->v1 = (1.241106190967544882e-2*input)+(0.97517787618064910582 * bw->v0);
    return bw->v0 + bw->v1;
}




/*------------------------------------------------Blood.c---------------------------------------------*/
uint16_t g_fft_index = 0;         	 	//fft输入输出下标

struct compx s1[FFT_N+16];           	//FFT输入和输出：从S[1]开始存放，根据大小自己定义
struct compx s2[FFT_N+16];           	//FFT输入和输出：从S[1]开始存放，根据大小自己定义

struct
{
	float 	Hp	;			//血红蛋白	
	float 	HpO2;			//氧合血红蛋白
	
}g_BloodWave;//血液波形数据

BloodData g_blooddata = {0};					//血液数据存储

#define CORRECTED_VALUE			47   			//标定血液氧气含量

/*funcation start ------------------------------------------------------------*/
//血液检测信息更新

void blood_data_update(void)
{

	//标志位被使能时 读取FIFO
	g_fft_index=0;
	while(g_fft_index < FFT_N)
	{
			//读取FIFO
			MAX30102_IIC_ReadReg(MAX30102_ADDRESS,REG_FIFO_DATA);  //read from MAX30102 FIFO2
			//将数据写入fft输入并清除输出
			if(g_fft_index < FFT_N)
			{
				//将数据写入fft输入并清除输出
				s1[g_fft_index].real = fifo_red;
				s1[g_fft_index].imag= 0;
				s2[g_fft_index].real = fifo_ir;
				s2[g_fft_index].imag= 0;
				g_fft_index++;
			}
	}
}

//血液信息转换
void blood_data_translate(void)
{	
	float n_denom;
	uint16_t i;
	//直流滤波
	float dc_red =0; 
	float dc_ir =0;
	float ac_red =0; 
	float ac_ir =0;
	
	for (i=0 ; i<FFT_N ; i++ ) 
	{
		dc_red += s1[i].real ;
		dc_ir +=  s2[i].real ;
	}
		dc_red =dc_red/FFT_N ;
		dc_ir =dc_ir/FFT_N ;
	for (i=0 ; i<FFT_N ; i++ )  
	{
		s1[i].real =  s1[i].real - dc_red ; 
		s2[i].real =  s2[i].real - dc_ir ; 
	}
	
	//移动平均滤波
	for(i = 1;i < FFT_N-1;i++) 
	{
			n_denom= ( s1[i-1].real + 2*s1[i].real + s1[i+1].real);
			s1[i].real=  n_denom/4.00; 
			
			n_denom= ( s2[i-1].real + 2*s2[i].real + s2[i+1].real);
			s2[i].real=  n_denom/4.00; 			
	}
	//八点平均滤波
	for(i = 0;i < FFT_N-8;i++) 
	{
			n_denom= ( s1[i].real+s1[i+1].real+ s1[i+2].real+ s1[i+3].real+ s1[i+4].real+ s1[i+5].real+ s1[i+6].real+ s1[i+7].real);
			s1[i].real=  n_denom/8.00; 
			
			n_denom= ( s2[i].real+s2[i+1].real+ s2[i+2].real+ s2[i+3].real+ s2[i+4].real+ s2[i+5].real+ s2[i+6].real+ s2[i+7].real);
			s2[i].real=  n_denom/8.00; 
			
	}
	
	//开始变换显示	
	g_fft_index = 0;	
	//快速傅里叶变换
	FFT(s1);
	FFT(s2);
	//解平方
	for(i = 0;i < FFT_N;i++) 
	{
		s1[i].real=sqrtf(s1[i].real*s1[i].real+s1[i].imag*s1[i].imag);
		s1[i].real=sqrtf(s2[i].real*s2[i].real+s2[i].imag*s2[i].imag);
	}
	//计算交流分量
	for (i=1 ; i<FFT_N ; i++ ) 
	{
		ac_red += s1[i].real ;
		ac_ir +=  s2[i].real ;
	}
	
	//读取峰值点的横坐标  结果的物理意义为 
	int s1_max_index = find_max_num_index(s1, 30);
	int s2_max_index = find_max_num_index(s2, 30);
	float Heart_Rate = 60.00 * ((100.0 * s1_max_index )/ 512.00)+20;
	
	g_blooddata.heart = Heart_Rate;
	
		float R = (ac_ir*dc_red)/(ac_red*dc_ir);
		float sp02_num =-45.060*R*R+ 30.354 *R + 94.845;
		g_blooddata.SpO2 = sp02_num;
			
}

uint16_t SPO2dataResult;  //用于存储最终要显示在血氧检测功能的一级菜单中的数据
uint16_t HeartdataResult; //用于存储最终要显示在心率检测功能的一级菜单中的数据
void blood_Loop(void)
{
	//血液信息获取
	blood_data_update();
	//血液信息转换
	blood_data_translate();
	//显示血液状态信息
	g_blooddata.SpO2 = (g_blooddata.SpO2 > 99.99) ? 99.99:g_blooddata.SpO2;
	SPO2dataResult = (uint16_t)g_blooddata.SpO2;	
	HeartdataResult = (uint16_t)g_blooddata.heart;
	
}

/*-----------------------------------------------------Blood.c-------------------------------------------------*/

