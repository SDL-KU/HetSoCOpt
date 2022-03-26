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


#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "parameters.h"
#include "initiate.h"


//#define PRINT_PROCESS


using namespace std;


int Schedule[120][NumStep];
int Sche=0;

#include "functions.h"

int main()
{
	int i=0, j=0, ii=0, jj=0, iii=0, jjj=0;
	int Part, Sched, Core0, Mem;
	int Best_Part, Best_Sche, Best_Mem, Best_Core;

	int CPU0_Free_Time = 0;
	int CPU1_Free_Time = 0;
	int Execution_Time = 0;
	int step_seq[NumStep]={0,};
	int NofStep[NumStep]={0,};

	clock_t START_CLK, STOP_CLK;

	SimResult Result[32][120][3][3];


	for(Part=0; Part < pow(2,NumStep); Part++)
	{
		for(Sched=0; Sched < factorial(NumStep); Sched++)
		{
			for(Core0=0; Core0<3; Core0++)
			{
				for(Mem=0; Mem < 3; Mem++)
				{
					Result[Part][Sched][Core0][Mem].Execution = 1000000;
				}
			}			
		}
	}

	for(i=0; i<NumStep; i++)
	{
		NofStep[i] = i;
	}

	Initial();
	Set_Schedule(NofStep, 0);

	START_CLK = clock();
	
//////////////////////////////////////////////////////////////
// Case Finder
	int Find_Part = 0;
	int Find_Sched = 0;
	int counter = 0;

	//Case 1
	
	int Find_Partition[NumStep]= {DMA, PS, DMA, DMA, PS}; 
	int Find_Schedule[NumStep] = {1,3,0,2,4};		//Each_core: 2
	//int Find_Schedule[NumStep] = {0,2,1,3,4};		//Each_core: 2
	//cout<<"=====  Case 1  ====="<<endl;
	// Step 1 --> Step 3 --> Step 5
	// Step 2 --> Step 4
	// Memory seperating
	//*/

	//Case 2
	/*/
	int Find_Partition[NumStep]= {DMA, PS, DMA, DMA, PS}; 
	int Find_Schedule[NumStep] = {1,3,0,2,4};		//Each_core: 2
	cout<<"=====  Case 2  ====="<<endl;
	// Step 1 --> Step 3 --> Step 5
	// Step 2 --> Step 4
	// ** Memory merging **
	//*/

	//Case 3
	/*/
	int Find_Partition[NumStep]= {DMA, PS, DMA, DMA, PS};
	int Find_Schedule[NumStep] = {2,0,4,3,1};		//Each_core: 2
	cout<<"=====  Case 3  ====="<<endl;
	// Step 3 --> Step 1
	// Step 5 --> Step 4 --> Step 2
	// Memory seperating
	//*/

	//Case 4
	//*
	//int Find_Partition[NumStep]= {DMA, PS, DMA, PS, PS};
	//int Find_Schedule[NumStep] = {0,3,2,1,4};		//Each_core: 2
	//cout<<"=====  Case 4  ====="<<endl;
	// Step 1 --> Step 4
	// Step 3 --> Step 2 --> Step 5
	// Memory seperating
	//*/

	// Optimum
	/*
	int Find_Partition[NumStep]= {DMA, PS, DMA, PS, PS};
	//int Find_Schedule[NumStep] = {2,1,0,4,3};		//Each_core: 2
	int Find_Schedule[NumStep] = {1,3,0,2,4};		//Each_core: 2
	cout<<"=====  Optimum  ====="<<endl;
	// Step 1 --> Step 4
	// Step 3 --> Step 2 --> Step 5
	// Memory seperating
	//*/

	for(i=0; i<NumStep; i++)
	{
		Find_Part += Find_Partition[i] * pow(2,4-i);
	}

	for(i=0; i<120; i++)
	{
		int Check = 0;
		for(j=0; j<NumStep; j++)
		{
			if(Schedule[i][j] == Find_Schedule[j])
			{
				Check++;
			}
		}
		if(Check == NumStep)
		{
			Find_Sched = i;
		}
	}
// Case Finder
//////////////////////////////////////////////////////////////
	
	
	for(Part=0; Part < pow(2,NumStep); Part++)
	{
		//cout<<"Part: "<<Part<<endl;

		for(Sched=0; Sched < factorial(NumStep); Sched++)
		{
			for(Core0=0; Core0<(float)NumStep/2; Core0++)
			{
				for(Mem=0; Mem < 3; Mem++)
				{
					counter++;

					int PS_CYCLE       = 0;
					int offset         = 0;
					int offset_density = 0;
					int overlap        = 0;

					int CPU0_Queue[NumStep]	 = {0,};
					int CPU0_Queue_point = 0;
					int CPU1_Queue[NumStep]	 = {0,};
					int CPU1_Queue_point = 0;

					// 초기화
					CPU0_Free_Time = 0;
					CPU1_Free_Time = 0;
					Execution_Time = 0;

					// Partition Setting
					for(i=0; i<NumStep; i++)
					{
						if((Part & (0x10>>i))    == 0)
							Step[i].Master = PS;
						else
							Step[i].Master = DMA; 
					}
					
					// Scheduling Setting
					for(i=0; i<NumStep; i++)
						step_seq[i] = Schedule[Sched][i];
					
					if(Mem == 0)
					{
						// Separated Memory
						DATA_A  =0;
						DATA_g  =1;
						DATA_T  =2;
						DATA_QR =3;
						DATA_r  =4;
						DATA_x  =5;
						DATA_y  =6;
						DntCare =7;
					}
					else if(Mem == 1)
					{
						// Shared Memory:A, other variables
						DATA_A  =0;
						DATA_g  =1;
						DATA_T  =1;
						DATA_QR =1;
						DATA_r  =1;
						DATA_x  =1;
						DATA_y  =1;
						DntCare =7;
					}
					else
					{
						// Shared Memory: all variables
						DATA_A  =0;
						DATA_g  =0;
						DATA_T  =0;
						DATA_QR =0;
						DATA_r  =0;
						DATA_x  =0;
						DATA_y  =0;
						DntCare =7;
					}

					for(i=0; i<NumStep; i++)
					{
						int step     = step_seq[i];
						int NofInfo  = 0;
						int info_now = 0;
						int NofIter  = 0;

						if(Step[step].Master == PS)
						{
							switch (step)
							{
							case 0: 
								PS_CYCLE = PS_TRANS_Cycle0;
								break;
							case 1:
								PS_CYCLE = PS_TRANS_Cycle1;
								break;
							case 2:
								PS_CYCLE = PS_TRANS_Cycle2;
								break;
							case 3:
								PS_CYCLE = PS_TRANS_Cycle3;
								break;
							case 4:
								PS_CYCLE = PS_TRANS_Cycle4;
								break;
							default:
								PS_CYCLE = 15;
								break;
							}

							// TRANSACTION Dynamic Allocation
							NofInfo = Step[step].NofInfo;
							Step[step].Data = new DATA[NofInfo];
							Step[step].Start= 0;
							Step[step].Time = 0;
							Step[step].Intrinsic_Time = 0;
							Step[step].NofData = NofInfo;

							//Step 
							for(j=0; j<NofInfo; j++)
							{
								Step[step].Data[j].Status = Step[step].Info[j].Status;
								
								if((Step[step].Data[j].Status)==BRAM_WRITE)			// Write at master position
								{
									int NofBEAT = Step[step].Info[j].NofBEAT;
									Step[step].Data[j].WTrans = new TRANSACTION[NofBEAT];

									Step[step].Data[j].Var       = Step[step].Info[j].Var;
									Step[step].Data[j].Start     = 0;
									Step[step].Data[j].Density   = Step[step].Info[j].Density;
									Step[step].Data[j].NofBEAT   = NofBEAT;
									Step[step].Data[j].NofWTrans = NofBEAT;
									Step[step].Data[j].NofRTrans = 0; // No Read transaction
									Step[step].Data[j].Addl_Time = Step[step].Info[j].Addl_Time_PS;

									for(ii=0; ii<NofBEAT; ii++)
									{
										Step[step].Data[j].WTrans[ii].Start     = 0;
										Step[step].Data[j].WTrans[ii].Time      = PS_CYCLE / (Step[step].Data[j].Density);
										Step[step].Data[j].WTrans[ii].NofBEAT   = 1;
									}

									Step[step].Data[j].Time      = ((Step[step].Data[j].NofWTrans) * PS_CYCLE) / (Step[step].Data[j].Density);
								}

								else if((Step[step].Data[j].Status)==BRAM_READ)		// Read at master position
								{
									int NofBEAT = Step[step].Info[j].NofBEAT;
									Step[step].Data[j].RTrans = new TRANSACTION[NofBEAT];

									Step[step].Data[j].Var       = Step[step].Info[j].Var;
									Step[step].Data[j].Start     = 0;
									Step[step].Data[j].Density   = Step[step].Info[j].Density;
									Step[step].Data[j].NofBEAT   = NofBEAT;
									Step[step].Data[j].NofRTrans = NofBEAT;
									Step[step].Data[j].NofWTrans = 0; // No Write transaction
									Step[step].Data[j].Addl_Time = Step[step].Info[j].Addl_Time_PS;

									for(ii=0; ii<NofBEAT; ii++)
									{
										Step[step].Data[j].RTrans[ii].Start     = 0;
										Step[step].Data[j].RTrans[ii].Time      = PS_CYCLE / (Step[step].Data[j].Density);
										Step[step].Data[j].RTrans[ii].NofBEAT   = 1;
									}

									Step[step].Data[j].Time      = ((Step[step].Data[j].NofRTrans) * PS_CYCLE) / (Step[step].Data[j].Density);									
								}

								else
								{   // No Transaction
									Step[step].Data[j].Start     = 0;
									Step[step].Data[j].Density   = Step[step].Info[j].Density;
									Step[step].Data[j].NofBEAT   = 0;
									Step[step].Data[j].NofWTrans = 0;
									Step[step].Data[j].NofRTrans = 0;
									Step[step].Data[j].Time      = 0;
									Step[step].Data[j].Addl_Time = Step[step].Info[j].Addl_Time_PS;
								}
							}
						}
						else
						{// DMA Part
							// TRANSACTION Dynamic Allocation
							NofInfo = Step[step].NofInfo;
							Step[step].Data = new DATA[NofInfo];
							Step[step].Start= 0;
							Step[step].Time = 0;
							Step[step].Intrinsic_Time = 0;
							Step[step].NofData = NofInfo;
							
							for(j=0; j<NofInfo; j++)
							{
								int NofBEAT = Step[step].Info[j].NofBEAT;
								int NofTrans= 0;

								Step[step].Data[j].Var       = Step[step].Info[j].Var;
								Step[step].Data[j].Start     = 0;
								Step[step].Data[j].Time	   = 0;
								Step[step].Data[j].Density   = Step[step].Info[j].Density*0.94;
								Step[step].Data[j].NofBEAT   = NofBEAT;
								Step[step].Data[j].Status    = Step[step].Info[j].Status;
								Step[step].Data[j].Addl_Time = Step[step].Info[j].Addl_Time_DMA;

								if(NofBEAT <= BURST_SIZE)
								{
									NofTrans = 1;
									Step[step].Data[j].NofRTrans = NofTrans;
									Step[step].Data[j].NofWTrans = NofTrans;
									Step[step].Data[j].RTrans = new TRANSACTION[NofTrans];
									Step[step].Data[j].WTrans = new TRANSACTION[NofTrans];
								}
								else
								{
									NofTrans = NofBEAT/BURST_SIZE;
									if((NofBEAT%BURST_SIZE) != 0)
									{
										NofTrans = NofTrans + 1;
									}

									Step[step].Data[j].NofRTrans = NofTrans;
									Step[step].Data[j].NofWTrans = NofTrans;

									Step[step].Data[j].RTrans = new TRANSACTION[NofTrans];
									Step[step].Data[j].WTrans = new TRANSACTION[NofTrans];
								}
								for(jj=0; jj<NofTrans; jj++)
								{
									Step[step].Data[j].RTrans[jj].Start     = 0;
									Step[step].Data[j].WTrans[jj].Start     = 0;

									if(NofBEAT < BURST_SIZE)
									{
										Step[step].Data[j].RTrans[jj].NofBEAT   = NofBEAT;
										Step[step].Data[j].WTrans[jj].NofBEAT   = NofBEAT;
										NofBEAT = 0;
									}
									else
									{
										Step[step].Data[j].RTrans[jj].NofBEAT   = BURST_SIZE;
										Step[step].Data[j].WTrans[jj].NofBEAT   = BURST_SIZE;
										NofBEAT = NofBEAT - BURST_SIZE;
									}

									Step[step].Data[j].RTrans[jj].Time      = ((Step[step].Data[j].RTrans[jj].NofBEAT) * DMA_TRANS_Cycle) / (Step[step].Data[j].Density);
									Step[step].Data[j].WTrans[jj].Time      = ((Step[step].Data[j].WTrans[jj].NofBEAT) * DMA_TRANS_Cycle) / (Step[step].Data[j].Density);
									
								}

								for(jj=0; jj<NofTrans; jj++)
								{
									if((Step[step].Data[j].Status)==BRAM_READ)
									{
										Step[step].Data[j].Time += Step[step].Data[j].RTrans[jj].Time;// ((Step[step].Data[j].RTrans[jj].NofBEAT) * DMA_TRANS_Cycle);
									}
									else if((Step[step].Data[j].Status)==BRAM_WRITE)
									{
										Step[step].Data[j].Time += Step[step].Data[j].WTrans[jj].Time;// ((Step[step].Data[j].WTrans[jj].NofBEAT) * DMA_TRANS_Cycle);
									}
								}

								if(((Step[step].Data[j].Status)==BRAM_WRITE) | ((Step[step].Data[j].Status)==BRAM_READ))
								{
									Step[step].Data[j].Time += DMA_SET + min((int)((Step[step].Data[j].NofBEAT)*DMA_TRANS_Cycle),DMA_LATENCY) + Step[step].Info[j].Addl_Time_DMA;
								}
								else
								{
									Step[step].Data[j].Time += Step[step].Info[j].Addl_Time_DMA;
								}							
								
							}

						}			

						for(j=0; j< (Step[step].NofData); j++)
						{
							
							//Step[step].Time += ((Step[step].Data[j].Time));
							vertex_intra_arrange(&Step[step]);
						}

					} // Step Iteration

					for(i=0; i<NumStep; i++)
						Step[i].Intrinsic_Time = Step[i].Time;

#ifdef PRINT_PROCESS
					cout<<endl<<"== Intrinsic Time =="<<endl;
					for(i=0; i<5; i++)
					{
						cout<<"Step "<<i+1<<" ";
						Print_Vertex(Step[i]);
					}
#endif

					////////////////////////////////////////////////////////////////////////////////
					//// Apply: CPU0, CPU1 의 시간에 맞게 vertex shift
					// CPU0, CPU1에 각각 할당.
					CPU0_Queue_point = 0;
					CPU1_Queue_point = 0;
					for(i=0; i<Core0; i++)
					{
						CPU0_Queue[CPU0_Queue_point] = step_seq[i];
						CPU0_Queue_point++;
					}
					for(; i<NumStep; i++)
					{
						CPU1_Queue[CPU1_Queue_point] = step_seq[i];
						CPU1_Queue_point++;
					}
					
					for(i=0; i<CPU0_Queue_point; i++)
					{
						int step    = CPU0_Queue[i];
						int NofData = Step[step].NofData;

						if(Step[step].Start < CPU0_Free_Time)
						{
							Step[step].Start = CPU0_Free_Time;
						}
						vertex_intra_arrange(&Step[step]);

						if(Step[step].Master == PS)
							CPU0_Free_Time = (Step[step].Start) + (Step[step].Time);
						else						
							CPU0_Free_Time = (Step[step].Data[NofData-1].Start) + DMA_SET;						
							
					}				

					for(i=0; i<CPU1_Queue_point; i++)
					{
						int step = CPU1_Queue[i];
						int NofData = Step[step].NofData;

						if(Step[step].Start < CPU1_Free_Time)
						{
							Step[step].Start = CPU1_Free_Time;
						}
	
						vertex_intra_arrange(&Step[step]);

						if(Step[step].Master == PS)
							CPU1_Free_Time = (Step[step].Start) + (Step[step].Time);
						else
							CPU1_Free_Time = (Step[step].Data[NofData-1].Start) + DMA_SET;
					}


#ifdef PRINT_PROCESS
					cout<<endl<<"== Vertex Shift =="<<endl;
					for(i=0; i<5; i++)
					{
						cout<<"Step "<<i+1<<" ";
						Print_Vertex(Step[i]);
					}
#endif

					//////////////////////////////////////////////////////////////////////////////
					// Apply: SAMD Conflict & Memory Conflict Iterative					
					int alternate=0;

					for(i=0; i<NumStep; i++)				// vertex
					{
						
						for(j=0; j<NumStep; j++)			// comparison vertex
						{
							
							int step         = step_seq[i];
							int cmp_step     = step_seq[j];

							int NofData      = Step[step].NofData;
							int comp_NofData = Step[cmp_step].NofData;

							int k=0;

							CPU0_Free_Time = 0;
							CPU1_Free_Time = 0;
							
							alternate = alternate % 2;

							for(k=0; k<CPU0_Queue_point; k++)
							{
								int step0    = CPU0_Queue[k];
								int NofData = Step[step0].NofData;

								if(Step[step0].Start < CPU0_Free_Time)
								{
									vertex_shift(&Step[step0],(CPU0_Free_Time-(Step[step0].Start)));
								}

								if(Step[step0].Master == PS)
									CPU0_Free_Time = (Step[step0].Start) + (Step[step0].Time);
								else
									CPU0_Free_Time = (Step[step0].Data[NofData-1].Start) + DMA_SET;
							}

							for(k=0; k<CPU1_Queue_point; k++)
							{
								int step1 = CPU1_Queue[k];
								int NofData = Step[step1].NofData;															

								if(Step[step1].Start < CPU1_Free_Time)
								{
									vertex_shift(&Step[step1],(CPU1_Free_Time-(Step[step1].Start)));
								}
								
								if(Step[step1].Master == PS)
									CPU1_Free_Time = (Step[step1].Start) + (Step[step1].Time);
								else
									CPU1_Free_Time = (Step[step1].Data[NofData-1].Start) + DMA_SET;
							}
							

							if(i != j)
							{
								if(Step[step].Master == DMA)
								{
									for(ii=0; ii<NofData; ii++)
									{
										for(jj=0; jj<comp_NofData; jj++)
										{
											int NofRTrans      = Step[step].Data[ii].NofRTrans;
											int NofWTrans      = Step[step].Data[ii].NofWTrans;

											int comp_NofRTrans = Step[cmp_step].Data[jj].NofRTrans;
											int comp_NofWTrans = Step[cmp_step].Data[jj].NofWTrans;

											int RFree=0;
											int WFree=0;
											
											if(ii>0)
											{
												if((Step[step].Data[ii-1].Status)==BRAM_READ)
												{
													RFree = (Step[step].Data[ii-1].Start) + (Step[step].Data[ii-1].Time);
													WFree = (Step[step].Data[ii-1].Start) + DMA_SET;
												}
												else if((Step[step].Data[ii-1].Status)==BRAM_WRITE)
												{
													RFree = (Step[step].Data[ii-1].Start) + DMA_SET;
													WFree = (Step[step].Data[ii-1].Start) + (Step[step].Data[ii-1].Time);
												}
												else
												{
													RFree = (Step[step].Data[ii-1].Start) + (Step[step].Data[ii-1].Time);
													WFree = (Step[step].Data[ii-1].Start) + (Step[step].Data[ii-1].Time);
												}

												if((Step[step].Data[ii].Status)==BRAM_READ)
												{
													Step[step].Data[ii].Start = RFree;
												}
												else if((Step[step].Data[ii].Status)==BRAM_WRITE)
												{
													Step[step].Data[ii].Start = WFree;

												}
												else
												{
													Step[step].Data[ii].Start = max(RFree,WFree);
												}																							
											}	

											if((Step[cmp_step].Master) == DMA)
											{
												if(jj>0)
												{
													if((Step[cmp_step].Data[jj-1].Status)==BRAM_READ)
													{
														RFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
														WFree = (Step[cmp_step].Data[jj-1].Start) + DMA_SET;
													}
													else if((Step[cmp_step].Data[jj-1].Status)==BRAM_WRITE)
													{
														RFree = (Step[cmp_step].Data[jj-1].Start) + DMA_SET;
														WFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
													}
													else
													{
														RFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
														WFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
													}

													if((Step[cmp_step].Data[jj].Status)==BRAM_READ)
													{
														Step[cmp_step].Data[jj].Start = RFree;
													}
													else if((Step[cmp_step].Data[jj].Status)==BRAM_WRITE)
													{
														Step[cmp_step].Data[jj].Start = WFree;

													}
													else
													{
														Step[cmp_step].Data[jj].Start = max(RFree,WFree);
													}
												}
											}
											else
											{
												if(jj>0)
												{
													if((Step[cmp_step].Data[jj-1].Status)==BRAM_READ)
													{
														RFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
														WFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
													}
													else if((Step[cmp_step].Data[jj-1].Status)==BRAM_WRITE)
													{
														RFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
														WFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
													}
													else
													{
														RFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
														WFree = (Step[cmp_step].Data[jj-1].Start) + (Step[cmp_step].Data[jj-1].Time);
													}

													if((Step[cmp_step].Data[jj].Status)==BRAM_READ)
													{
														Step[cmp_step].Data[jj].Start = RFree;
														if(RFree > (Step[cmp_step].Data[jj].RTrans[0].Start))
														{
															RTransaction_offset(&Step[cmp_step], jj, 0, RFree - (Step[cmp_step].Data[jj].RTrans[0].Start));
														}
														
													}
													else if((Step[cmp_step].Data[jj].Status)==BRAM_WRITE)
													{
														Step[cmp_step].Data[jj].Start = WFree;

													}
													else
													{
														Step[cmp_step].Data[jj].Start = max(RFree,WFree);
													}
												}
											}
	
											for(iii=0; iii<NofRTrans; iii++)
											{
												for(jjj=0; jjj<comp_NofRTrans; jjj++)
												{
													if((Step[cmp_step].Master) == DMA) // Memory Conflict
													{														
														if((*(Step[step].Data[ii].Var)) == (*(Step[cmp_step].Data[jj].Var)))	// same variable is requested.
														{
															//cout<<"i,j: "<<i<<" "<<j<<" ii,jj: "<<ii<<" "<<jj<<" iii,jjj: "<<iii<<" "<<jjj<<"NofTrans: "<<NofRTrans<<" "<<comp_NofRTrans<<" "<<NofWTrans<<" "<<comp_NofWTrans<<endl;
															// DMA - DMA Conflict
															if((Step[step].Data[ii].RTrans[iii].Start) < (Step[cmp_step].Data[jj].RTrans[jjj].Start))
															{
																if( ((Step[step].Data[ii].RTrans[iii].Start)+(Step[step].Data[ii].RTrans[iii].Time)) > (Step[cmp_step].Data[jj].RTrans[jjj].Start) ) // conflict happens
																{
																	offset = ((Step[step].Data[ii].RTrans[iii].Start)+(Step[step].Data[ii].RTrans[iii].Time)) - (Step[cmp_step].Data[jj].RTrans[jjj].Start);
																	offset_density = offset * (Step[step].Data[ii].Density) * (Step[cmp_step].Data[jj].Density);

																	if(offset > 0)
																	{
																		RTransaction_offset(&Step[cmp_step], jj, jjj, offset_density);
																	}
																}																
															}
															else if((Step[step].Data[ii].RTrans[iii].Start) > (Step[cmp_step].Data[jj].RTrans[jjj].Start))
															{
																if( ((Step[cmp_step].Data[jj].RTrans[jjj].Start)+(Step[cmp_step].Data[jj].RTrans[jjj].Time)) > (Step[step].Data[ii].RTrans[iii].Start) ) // conflict happens
																{
																	offset = ((Step[cmp_step].Data[jj].RTrans[jjj].Start)+(Step[cmp_step].Data[jj].RTrans[jjj].Time)) - (Step[step].Data[ii].RTrans[iii].Start);
																	offset_density = offset * (Step[step].Data[ii].Density) * (Step[cmp_step].Data[jj].Density);

																	if(offset > 0)
																	{
																		RTransaction_offset(&Step[step], ii, iii, offset_density);
																	}
																}																
															}
															else // Transactions start simultaneously
															{
																
																if((alternate%2) == 0)
																{
																	//cout<<"1i,j: "<<i<<" "<<j<<" ii,jj: "<<ii<<" "<<jj<<" iii,jjj: "<<iii<<" "<<jjj<<"NofTrans: "<<NofRTrans<<" "<<comp_NofRTrans<<" "<<NofWTrans<<" "<<comp_NofWTrans<<endl;
																	RTransaction_offset(&Step[step], ii, iii, Step[cmp_step].Data[jj].RTrans[jjj].Time);
																	alternate++;
																}
																else
																{
																	//cout<<"2i,j: "<<i<<" "<<j<<" ii,jj: "<<ii<<" "<<jj<<" iii,jjj: "<<iii<<" "<<jjj<<"NofTrans: "<<NofRTrans<<" "<<comp_NofRTrans<<" "<<NofWTrans<<" "<<comp_NofWTrans<<endl;
																	RTransaction_offset(&Step[cmp_step], jj, jjj, Step[step].Data[ii].RTrans[iii].Time);
																	alternate++;
																}																
															}
															
														}
													}
													else   // Step[comp_step].Master == PS
													{
														// DMA - PS conflict
														// consider SAMD, memory conflict both
														if((Step[step].Data[ii].Status) == BRAM_READ) // SAMD Conflict happens when DMA read  from bram only
														{											
															if((Step[cmp_step].Data[jj].Status) == BRAM_READ)	// PS Read
															{
																if((*(Step[step].Data[ii].Var)) == (*(Step[cmp_step].Data[jj].Var)))	// SAMD + Memory Conflict
																{																	
																	if((Step[step].Data[ii].RTrans[iii].Start) < (Step[cmp_step].Data[jj].RTrans[jjj].Start))	// PS가 더 늦게 시작하면.
																	{
																		offset = ((Step[step].Data[ii].RTrans[iii].Start)+(Step[step].Data[ii].RTrans[iii].Time)) - (Step[cmp_step].Data[jj].RTrans[jjj].Start);
																		offset_density = offset * (Step[step].Data[ii].Density) * (Step[cmp_step].Data[jj].Density);

																		if(offset > 0)
																		{																			
																			if((iii+1) < NofRTrans)//if((iii==1)|(iii==0))//
																			{
																				RTransaction_offset(&Step[cmp_step], jj, jjj, offset_density + (Step[step].Data[ii].RTrans[iii+1].Time));
																			}
																			else
																			{																			
																				RTransaction_offset(&Step[cmp_step], jj, jjj, offset_density);																			
																			}
																		}
																	}
																}
																else   // SAMD Conflict
																{																	
																	if((Step[step].Data[ii].RTrans[iii].Start) < (Step[cmp_step].Data[jj].RTrans[jjj].Start))
																	{
																		offset = ((Step[step].Data[ii].RTrans[iii].Start)+(Step[step].Data[ii].RTrans[iii].Time)) - (Step[cmp_step].Data[jj].RTrans[jjj].Start);
																		offset_density = offset * (Step[step].Data[ii].Density) * (Step[cmp_step].Data[jj].Density);
																			
																		if(offset > 0)
																		{																		
																			RTransaction_offset(&Step[cmp_step], jj, jjj, offset_density);	
																		}
																	}
																}
															}
														}
														if((Step[step].Data[ii].Status) == BRAM_WRITE) // SAMD Conflict happens when DMA read  from bram only
														{
															if((Step[cmp_step].Data[jj].Status) == BRAM_WRITE)	// PS Read
															{
																// there is No Conflict b/c hardware can handle multiple-outstanding.
															}
														}
													}	
													// Data vertex time arrange

													if((Step[step].Data[ii].Time) < ((Step[step].Data[ii].WTrans[iii].Start) + (Step[step].Data[ii].WTrans[iii].Time) + (Step[step].Data[ii].Addl_Time) - (Step[step].Data[ii].Start)))
													{												
														Step[step].Data[ii].Time = ((Step[step].Data[ii].WTrans[iii].Start) + (Step[step].Data[ii].WTrans[iii].Time) + (Step[step].Data[ii].Addl_Time) - (Step[step].Data[ii].Start));
													}

													if((Step[cmp_step].Master) == PS)
													{
														if((Step[cmp_step].Data[jj].Status)== BRAM_READ)
														{
															if((Step[cmp_step].Data[jj].Time) < ((Step[cmp_step].Data[jj].RTrans[jjj].Start) + (Step[cmp_step].Data[jj].RTrans[jjj].Time) + (Step[cmp_step].Data[jj].Addl_Time) - (Step[cmp_step].Data[jj].Start)))
															{
																Step[cmp_step].Data[jj].Time = (Step[cmp_step].Data[jj].RTrans[jjj].Start) + (Step[cmp_step].Data[jj].RTrans[jjj].Time) + (Step[cmp_step].Data[jj].Addl_Time) - (Step[cmp_step].Data[jj].Start);
															}															
														}													
														else if((Step[cmp_step].Data[jj].Status)== BRAM_WRITE)
														{
															if((Step[cmp_step].Data[jj].Time)<((Step[cmp_step].Data[jj].WTrans[jjj].Start) + (Step[cmp_step].Data[jj].WTrans[jjj].Time) + (Step[cmp_step].Data[jj].Addl_Time) - (Step[cmp_step].Data[jj].Start)))
															{
																Step[cmp_step].Data[jj].Time = (Step[cmp_step].Data[jj].WTrans[jjj].Start) + (Step[cmp_step].Data[jj].WTrans[jjj].Time) + (Step[cmp_step].Data[jj].Addl_Time) - (Step[cmp_step].Data[jj].Start);
															}															
														}													
														else
														{
															Step[cmp_step].Data[jj].Time = (Step[cmp_step].Data[jj].Addl_Time);
														}

													}
													else
													{
														if((Step[cmp_step].Data[jj].Time) < ((Step[cmp_step].Data[jj].WTrans[jjj].Start) + (Step[cmp_step].Data[jj].WTrans[jjj].Time) + (Step[cmp_step].Data[jj].Addl_Time) - (Step[cmp_step].Data[jj].Start)))
														{
															Step[cmp_step].Data[jj].Time = (Step[cmp_step].Data[jj].WTrans[jjj].Start) + (Step[cmp_step].Data[jj].WTrans[jjj].Time) + (Step[cmp_step].Data[jj].Addl_Time) - (Step[cmp_step].Data[jj].Start);
														}

													}
												}
											}

											if((Step[step].Time) < ((Step[step].Data[ii].Start) + (Step[step].Data[ii].Time) - (Step[step].Start)))
											{
												Step[step].Time = (Step[step].Data[ii].Start) + (Step[step].Data[ii].Time) - (Step[step].Start);	
												//cout<<"Step: "<<step<<" "<<Step[step].Time<<" = "<<(Step[step].Data[ii].Start)<<" + "<<(Step[step].Data[ii].Time)<<" - "<<(Step[step].Start)<<endl;
											}

											if((Step[cmp_step].Time) < ((Step[cmp_step].Data[jj].Start) + (Step[cmp_step].Data[jj].Time) - (Step[cmp_step].Start)))
											{
												Step[cmp_step].Time = (Step[cmp_step].Data[jj].Start) + (Step[cmp_step].Data[jj].Time) - (Step[cmp_step].Start);
												//cout<<"comp_Step: "<<comp_step<<" "<<Step[comp_step].Time<<" = "<<(Step[comp_step].Data[jj].Start)<<" + "<<(Step[comp_step].Data[jj].Time)<<" - "<<(Step[comp_step].Start)<<endl;
											}

										}
									}
									
								}
							}													
						}
					}

#ifdef PRINT_PROCESS
					cout<<endl<<"== Data Conflict =="<<endl;
					for(i=0; i<5; i++)
					{
						cout<<"Step "<<i+1<<" ";
						Print_Vertex(Step[i]);
					}
#endif

					// End Simulation
					// And Store Result

					int ExTime=0;
					for(i=0; i<NumStep; i++)
					{
						Result[Part][Sched][Core0][Mem].Start[i]  = Step[i].Start;
						Result[Part][Sched][Core0][Mem].Time[i]   = Step[i].Time;
						Result[Part][Sched][Core0][Mem].Intrinsic_Time[i]   = Step[i].Intrinsic_Time;

						if(ExTime < (Step[i].Start + Step[i].Time) )
						{
							ExTime = (Step[i].Start + Step[i].Time);
						}
					}

					Result[Part][Sched][Core0][Mem].Execution = ExTime;
				}
			}
			
		}
	}

	STOP_CLK = clock();
	int Min_Time = INT_MAX;

	ofstream fP0("File0.txt");
	ofstream fP1("File1.txt");
	ofstream fP2("File2.txt");
	ofstream fP3("File3.txt");
	ofstream fP4("File4.txt");
	ofstream fP5("File5.txt");
	ofstream fP6("File6.txt");

	counter = 0;
	for (Part = 0; Part < pow(2, NumStep); Part++)
	{
		if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) < 6) // Number of DMA
		{
			for (Sched = 0; Sched < factorial(NumStep); Sched++)
			{
				for (Core0 = 0; Core0 < (float)NumStep / 2; Core0++)
				{
					for (Mem = 0; Mem < 3; Mem++)
					{
						counter++;
						fP0 << counter << endl;
						for (i = 0; i<NumStep; i++)
						{
								fP0 << "Step " << i + 1 << " Start: " << Result[Part][Sched][Core0][Mem].Start[i]
								<< " Time : " << Result[Part][Sched][Core0][Mem].Time[i]
								<< " (" << Result[Part][Sched][Core0][Mem].Intrinsic_Time[i] << ") Exec.time: " << Result[Part][Sched][Core0][Mem].Start[i] + Result[Part][Sched][Core0][Mem].Time[i] << endl;
						}
						fP0 << endl;

						if ((Result[Part][Sched][Core0][Mem].Execution) < Min_Time) {

							Min_Time = Result[Part][Sched][Core0][Mem].Execution;

							Best_Part = Part;
							Best_Sche = Sched;
							Best_Core = Core0;
							Best_Mem = Mem;
						}

						if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) == 0)
						{
							int tmp = 0;
							fP1 <<"( " << counter << ") " << "PT: ";
							for (int k = 0; k<NumStep; k++)
							{
								if ((Part & (0x10 >> k)) == 0) {
									tmp = PS;
									fP1 << tmp << "PS ";
								}
								else {
									tmp = DMA;
									fP1 << tmp << "DMA ";
								}
							}

							fP1 << "Schedule: ";
							for (int k = 0; k < 5; k++) {
								fP1 << Schedule[Sched][k] << " ";
							}

							fP1 << "CPU 0: ";
							for (int k = 0; k<Core0; k++) {
								fP1 << step_seq[k] + 1 << " ";
							}

							fP1 << "CPU 1: ";
							for (int k = Core0; k<NumStep; k++) {
								fP1 << step_seq[k] + 1 << " ";
							}

							fP1 << "Memory: " << Mem << " ";
							fP1 << Result[Part][Sched][Core0][Mem].Execution << endl;
						}
						else if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) == 1)
						{
							int tmp = 0;
							fP2 << "( " << counter << ") " << "PT: ";
							for (int k = 0; k<NumStep; k++)
							{
								if ((Part & (0x10 >> k)) == 0) {
									tmp = PS;
									fP2 << tmp << "PS ";
								}
								else {
									tmp = DMA;
									fP2 << tmp << "DMA ";
								}
							}

							fP2 << "Schedule: ";
							for (int k = 0; k < 5; k++) {
								fP2 << Schedule[Sched][k] << " ";
							}

							fP2 << "CPU 0: ";
							for (int k = 0; k<Core0; k++) {
								fP2 << step_seq[k] + 1 << " ";
							}

							fP2 << "CPU 1: ";
							for (int k = Core0; k<NumStep; k++) {
								fP2 << step_seq[k] + 1 << " ";
							}

							fP2 << "Memory: " << Mem << " ";
							fP2 << Result[Part][Sched][Core0][Mem].Execution << endl;
						}
						else if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) == 2)
						{
							int tmp = 0;
							fP3 << "( " << counter << ") " << "PT: ";
							for (int k = 0; k<NumStep; k++)
							{
								if ((Part & (0x10 >> k)) == 0) {
									tmp = PS;
									fP3 << tmp << "PS ";
								}
								else {
									tmp = DMA;
									fP3 << tmp << "DMA ";
								}
							}

							fP3 << "Schedule: ";
							for (int k = 0; k < 5; k++) {
								fP3 << Schedule[Sched][k] << " ";
							}

							fP3 << "CPU 0: ";
							for (int k = 0; k<Core0; k++) {
								fP3 << step_seq[k] + 1 << " ";
							}

							fP3 << "CPU 1: ";
							for (int k = Core0; k<NumStep; k++) {
								fP3 << step_seq[k] + 1 << " ";
							}

							fP3 << "Memory: " << Mem << " ";
							fP3 << Result[Part][Sched][Core0][Mem].Execution << endl;
						}
						else if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) == 3)
						{
							int tmp = 0;
							fP4 << "( " << counter << ") " << "PT: ";
							for (int k = 0; k<NumStep; k++)
							{
								if ((Part & (0x10 >> k)) == 0) {
									tmp = PS;
									fP4 << tmp << "PS ";
								}
								else {
									tmp = DMA;
									fP4 << tmp << "DMA ";
								}
							}

							fP4 << "Schedule: ";
							for (int k = 0; k < 5; k++) {
								fP4 << Schedule[Sched][k] << " ";
							}

							fP4 << "CPU 0: ";
							for (int k = 0; k<Core0; k++) {
								fP4 << step_seq[k] + 1 << " ";
							}

							fP4 << "CPU 1: ";
							for (int k = Core0; k<NumStep; k++) {
								fP4 << step_seq[k] + 1 << " ";
							}

							fP4 << "Memory: " << Mem << " ";
							fP4 << Result[Part][Sched][Core0][Mem].Execution << endl;
						}
						else if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) == 4)
						{
							int tmp = 0;
							fP5 << "( " << counter << ") " << "PT: ";
							for (int k = 0; k<NumStep; k++)
							{
								if ((Part & (0x10 >> k)) == 0) {
									tmp = PS;
									fP5 << tmp << "PS ";
								}
								else {
									tmp = DMA;
									fP5 << tmp << "DMA ";
								}
							}

							fP5 << "Schedule: ";
							for (int k = 0; k < 5; k++) {
								fP5 << Schedule[Sched][k] << " ";
							}

							fP5 << "CPU 0: ";
							for (int k = 0; k<Core0; k++) {
								fP5 << step_seq[k] + 1 << " ";
							}

							fP5 << "CPU 1: ";
							for (int k = Core0; k<NumStep; k++) {
								fP5 << step_seq[k] + 1 << " ";
							}

							fP5 << "Memory: " << Mem << " ";
							fP5 << Result[Part][Sched][Core0][Mem].Execution << endl;
						}
						else if ((((Part & 0x10) >> 4) + ((Part & 0x8) >> 3) + ((Part & 0x4) >> 2) + ((Part & 0x2) >> 1) + ((Part & 0x1))) == 5)
						{
							int tmp = 0;
							fP6 << "( " << counter << ") " << "PT: ";
							for (int k = 0; k<NumStep; k++)
							{
								if ((Part & (0x10 >> k)) == 0) {
									tmp = PS;
									fP6 << tmp << "PS ";
								}
								else {
									tmp = DMA;
									fP6 << tmp << "DMA ";
								}
							}

							fP6 << "Schedule: ";
							for (int k = 0; k < 5; k++) {
								fP6 << Schedule[Sched][k] << " ";
							}

							fP6 << "CPU 0: ";
							for (int k = 0; k<Core0; k++) {
								fP6 << step_seq[k] + 1 << " ";
							}

							fP6 << "CPU 1: ";
							for (int k = Core0; k<NumStep; k++) {
								fP6 << step_seq[k] + 1 << " ";
							}

							fP6 << "Memory: " << Mem << " ";
							fP6 << Result[Part][Sched][Core0][Mem].Execution << endl;
						}
					}
				}
			}
		}
	}
	fP0.close();
	fP1.close();
	fP2.close();
	fP3.close();
	fP4.close();
	fP5.close();
	fP6.close();

	if (Min_Time != INT_MAX)
	{
		cout << "==================================================" << endl;

		cout << "Execution Time: " << Result[Best_Part][Best_Sche][Best_Core][Best_Mem].Execution << endl << endl;

		for (i = 0; i<NumStep; i++)
		{
			cout << "Step " << i + 1 << " Start: " << Result[Best_Part][Best_Sche][Best_Core][Best_Mem].Start[i]
				<< " Time : " << Result[Best_Part][Best_Sche][Best_Core][Best_Mem].Time[i]
				<< " (" << Result[Best_Part][Best_Sche][Best_Core][Best_Mem].Intrinsic_Time[i] << ") Exec.time: " << Result[Best_Part][Best_Sche][Best_Core][Best_Mem].Start[i] + Result[Best_Part][Best_Sche][Best_Core][Best_Mem].Time[i] << endl;
		}
		cout << endl;
		cout << "== BEST partition: ";
		for (i = 0; i<NumStep; i++)
		{
			if ((Best_Part & (0x10 >> i)) == 0)
				cout << "PS - ";
			else
				cout << "DMA - ";
		}
		cout << endl;

		cout << "== BEST schedule: " << Best_Sche;

		for (i = 0; i<NumStep; i++)
		{
			step_seq[i] = Schedule[Best_Sche][i];
		}
		cout << endl;
		cout << "    CPU0 Schedule: ";

		for (i = 0; i<Best_Core; i++)
		{
			cout << step_seq[i] + 1 << " ";
		}
		cout << endl;
		cout << "    CPU1 Schedule: ";
		for (i = Best_Core; i<NumStep; i++)
		{
			cout << step_seq[i] + 1 << " ";
		}
		cout << endl;

		cout << "== BEST Memory: " << Best_Mem << endl;
		if (Best_Mem == 0)
		{
			cout << "Separated Memory" << endl;
		}
		else if (Best_Mem == 1)
		{
			cout << "Shared Memory: A, other variables" << endl;
		}
		else
		{
			cout << "Shared Memory: all variables" << endl;
		}
	}
	else
	{
		cout << "Could not find optimal setting" << endl;
		cout << "Check program and parameters" << endl;
		cout << "1. Number of DMA: Example case may have less number of DMA than if condition" << endl;
		cout << "2. Memory set: To memory conflict test, memory number?" << endl;
	}


	cout << "Simulation time: " << (float)(STOP_CLK - START_CLK) / 1000 << "s" << endl;

	for (i = 0; i<NumStep; i++)
	{
		delete[] Step[i].Info;
		delete[] Step[i].Data;
	}

}
