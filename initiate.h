/******************************************************************************
Copyright (c) 2022 SoC Design Laboratory, Konkuk University, South Korea
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met: redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer;
redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution;
neither the name of the copyright holders nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Authors: Jooho Wang (joohowang@konkuk.ac.kr)

Revision History
2022.03.25: Started by Jooho Wang
*******************************************************************************/

#define PS_TRANS_Cycle0  26
#define PS_TRANS_Cycle1  26
#define PS_TRANS_Cycle2  48
#define PS_TRANS_Cycle3  36
#define PS_TRANS_Cycle4  29
#define DMA_TRANS_Cycle  1


#define NumStep 5

// Data Set = memory initial location.
// location will be changed while memory mapping swap
int	DATA_A  =0;
int DATA_g  =1;
int DATA_T  =2;
int DATA_QR =3;
int DATA_r  =4;
int DATA_x  =5;
int DATA_y  =6;
int DntCare =7;

VERTEX Step[NumStep];
//VERTEX Step1, Step2, Step4, Step5, Step6;	// °¢ Step[ ±âº» vertex.

void Initial()
{
	Step[0].Start                 = 0;
	Step[0].Time                  = 0;
	Step[0].NofInfo	            = 3;
	Step[0].Info                  = new INFO[3]; // Data set information for Step[ 1
	// Default
	Step[0].Info[0].Var		    = &DATA_r;
	Step[0].Info[0].Status		= BRAM_READ;
	Step[0].Info[0].NofBEAT		= 30;
	Step[0].Info[0].Addl_Time_PS	= 0;
	Step[0].Info[0].Addl_Time_DMA = 10;
	Step[0].Info[0].Density	    = 1;
	Step[0].Info[1].Var		    = &DATA_A;
	Step[0].Info[1].Status	    = BRAM_READ;
	Step[0].Info[1].NofBEAT		= 1920;
	Step[0].Info[1].Addl_Time_PS  = 0;
	Step[0].Info[1].Addl_Time_DMA = 0;
	Step[0].Info[1].Density	    = 1;
	Step[0].Info[2].Var		    = &DATA_g;
	Step[0].Info[2].Status		= BRAM_WRITE;
	Step[0].Info[2].NofBEAT		= 60;
	Step[0].Info[2].Addl_Time_PS	= 0;
	Step[0].Info[2].Addl_Time_DMA = 0;
	Step[0].Info[2].Density	    = (float)1/30;

	Step[1].Start                 = 0;
	Step[1].Time                  = 0;
	Step[1].Master                = PS;
	Step[1].NofInfo	            = 2;
	Step[1].Info                  = new INFO[2];

	Step[1].Info[0].Var		    = &DATA_g;
	Step[1].Info[0].Status		= BRAM_READ;
	Step[1].Info[0].NofBEAT		= 60;
	Step[1].Info[0].Addl_Time_PS  = 0;
	Step[1].Info[0].Addl_Time_DMA = 0;
	Step[1].Info[0].Density	    = 1;
	Step[1].Info[1].Var		    = &DATA_T;
	Step[1].Info[1].Status		= BRAM_WRITE;
	Step[1].Info[1].NofBEAT		= 5;
	Step[1].Info[1].Addl_Time_PS	= 0;
	Step[1].Info[1].Addl_Time_DMA = 0;
	Step[1].Info[1].Density	    = 1;
	
	Step[2].Start                 = 0;
	Step[2].Time                  = 0;
	Step[2].Master                = PS;
	Step[2].NofInfo	            = 8;
	Step[2].Info                  = new INFO[8];

	Step[2].Info[0].Var		    = &DATA_QR;
	Step[2].Info[0].Status        = BRAM_READ;
	Step[2].Info[0].NofBEAT		= 120;
	Step[2].Info[0].Addl_Time_PS	= 0;
	Step[2].Info[0].Addl_Time_DMA = 10;
	Step[2].Info[0].Density	    = 1;
	Step[2].Info[1].Var		    = &DATA_QR;
	Step[2].Info[1].Status        = BRAM_READ;
	Step[2].Info[1].NofBEAT		= 30;
	Step[2].Info[1].Addl_Time_PS	= 0;
	Step[2].Info[1].Addl_Time_DMA = 10;
	Step[2].Info[1].Density	    = 1;
	Step[2].Info[2].Var		    = &DATA_A;
	Step[2].Info[2].Status        = BRAM_READ;
	Step[2].Info[2].NofBEAT		= 30;
	Step[2].Info[2].Addl_Time_PS  = 0;
	Step[2].Info[2].Addl_Time_DMA = 10;
	Step[2].Info[2].Density	    = 1;
	Step[2].Info[3].Var		    = &DATA_y;
	Step[2].Info[3].Status        = BRAM_READ;
	Step[2].Info[3].NofBEAT		= 30;
	Step[2].Info[3].Addl_Time_PS	= 0;
	Step[2].Info[3].Addl_Time_DMA = 10;
	Step[2].Info[3].Density	    = 1;
	Step[2].Info[4].Var		    = &DntCare;
	Step[2].Info[4].Status        = COMPUTE;
	Step[2].Info[4].NofBEAT		= 0;
	Step[2].Info[4].Addl_Time_PS  = 0;
	Step[2].Info[4].Addl_Time_DMA = 500;
	Step[2].Info[4].Density	    = 1;
	Step[2].Info[5].Var		    = &DATA_QR;
	Step[2].Info[5].Status        = BRAM_WRITE;
	Step[2].Info[5].NofBEAT		= 150;
	Step[2].Info[5].Addl_Time_PS  = 0;
	Step[2].Info[5].Addl_Time_DMA = 10;
	Step[2].Info[5].Density	    = 1;
	Step[2].Info[6].Var		    = &DATA_QR;
	Step[2].Info[6].Status        = BRAM_WRITE;
	Step[2].Info[6].NofBEAT		= 30;
	Step[2].Info[6].Addl_Time_PS	= 0;
	Step[2].Info[6].Addl_Time_DMA = 10;
	Step[2].Info[6].Density	    = 1;
	Step[2].Info[7].Var		    = &DATA_x;
	Step[2].Info[7].Status        = BRAM_WRITE;
	Step[2].Info[7].NofBEAT		= 5;
	Step[2].Info[7].Addl_Time_PS	= 0;
	Step[2].Info[7].Addl_Time_DMA = 0;
	Step[2].Info[7].Density	    = 1;


	Step[3].Start                 = 0;
	Step[3].Time                  = 0;
	Step[3].Master                = PS;
	Step[3].NofInfo	            = 8;
	Step[3].Info                  = new INFO[8];

	Step[3].Info[0].Var	        = &DATA_x;
	Step[3].Info[0].Status        = BRAM_READ;
	Step[3].Info[0].NofBEAT		= 5;
	Step[3].Info[0].Addl_Time_PS  = 0;
	Step[3].Info[0].Addl_Time_DMA = 10;
	Step[3].Info[0].Density	    = 1;
	Step[3].Info[1].Var		    = &DATA_A;
	Step[3].Info[1].Status        = BRAM_READ;
	Step[3].Info[1].NofBEAT		= 30;
	Step[3].Info[1].Addl_Time_PS  = 0;
	Step[3].Info[1].Addl_Time_DMA = 15;
	Step[3].Info[1].Density	    = 1;
	Step[3].Info[2].Var		    = &DATA_A;
	Step[3].Info[2].Status        = BRAM_READ;
	Step[3].Info[2].NofBEAT		= 30;
	Step[3].Info[2].Addl_Time_PS  = 0;
	Step[3].Info[2].Addl_Time_DMA = 15;
	Step[3].Info[2].Density	    = 1;
	Step[3].Info[3].Var		    = &DATA_A;
	Step[3].Info[3].Status        = BRAM_READ;
	Step[3].Info[3].NofBEAT		= 30;
	Step[3].Info[3].Addl_Time_PS	= 0;
	Step[3].Info[3].Addl_Time_DMA = 15;
	Step[3].Info[3].Density	    = 1;
	Step[3].Info[4].Var		    = &DATA_A;
	Step[3].Info[4].Status        = BRAM_READ;
	Step[3].Info[4].NofBEAT		= 30;
	Step[3].Info[4].Addl_Time_PS  = 0;
	Step[3].Info[4].Addl_Time_DMA = 15;
	Step[3].Info[4].Density	    = 1;
	Step[3].Info[5].Var		    = &DATA_A;
	Step[3].Info[5].Status        = BRAM_READ;
	Step[3].Info[5].NofBEAT		= 30;
	Step[3].Info[5].Addl_Time_PS  = 0;
	Step[3].Info[5].Addl_Time_DMA = 15;
	Step[3].Info[5].Density	    = 1;
	Step[3].Info[6].Var		    = &DATA_y;
	Step[3].Info[6].Status		= BRAM_READ;
	Step[3].Info[6].NofBEAT		= 30;
	Step[3].Info[6].Addl_Time_PS  = 0;
	Step[3].Info[6].Addl_Time_DMA = 0;
	Step[3].Info[6].Density	    = 1;
	Step[3].Info[7].Var		    = &DATA_r;
	Step[3].Info[7].Status        = BRAM_WRITE;
	Step[3].Info[7].NofBEAT		= 30;
	Step[3].Info[7].Addl_Time_PS  = 0;
	Step[3].Info[7].Addl_Time_DMA = 0;
	Step[3].Info[7].Density	    = 1;

	Step[4].Start                 = 0;
	Step[4].Time                  = 0;
	Step[4].Master                = PS;
	Step[4].NofInfo	            = 1;
	Step[4].Info                  = new INFO[1];

	Step[4].Info[0].Var		    = &DATA_r;
	Step[4].Info[0].Status		= BRAM_READ;
	Step[4].Info[0].NofBEAT		= 30;
	Step[4].Info[0].Addl_Time_PS  = 0;
	Step[4].Info[0].Addl_Time_DMA = 10;
	Step[4].Info[0].Density	    = 1;
}
