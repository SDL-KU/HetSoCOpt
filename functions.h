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

int factorial(int x)
{
	int p=0;
	int y=1;

	for(p=x; p>0; p--)
	{
		y=y*p;
	}

	return y;
}

void Set_Schedule(int *step, int n) {
    int p, temp;

    if(n==4) 
	{
        for(p=0;p<NumStep;p++)
		{
			Schedule[Sche][p]=step[p];
		}
		Sche++;
        return;
    }
    for(p=n;p<NumStep;p++) 
	{
        temp=step[n];
		step[n]=step[p];
		step[p]=temp;
        Set_Schedule(step, n+1);
    }
    temp=step[n];
    for(p=n;p<NumStep-1;p++)
	{
		step[p]=step[p+1];
	}
    step[NumStep-1]=temp;
}

void vertex_intra_arrange(VERTEX *Vtx)
{
	int i=0, j=0;
	int NofData = Vtx->NofData;

	int ExecTime_R=0, ExecTime_W=0, ExecTime=0;
	int RFree=0, WFree=0, Free=0;
	int Time_Sum=0, Time_SumR=0, Time_SumW=0;

	// Data struct안에 있는 transaction 정리.
	Vtx->Data[0].Start = Vtx->Start;

	if(Vtx->Master == PS)
	{
		for(i=0; i<NofData; i++)
		{
			int NofWTrans = Vtx->Data[i].NofWTrans;
			int NofRTrans = Vtx->Data[i].NofRTrans;

			if((Vtx->Data[i].Status) == BRAM_READ)			// READ
			{
				Vtx->Data[i].RTrans[0].Start = (Vtx->Data[i].Start);

				Time_Sum = 0;
				for(j=1; j<NofRTrans; j++)
				{
					Vtx->Data[i].RTrans[j].Start = (Vtx->Data[i].RTrans[j-1].Start) + (Vtx->Data[i].RTrans[j-1].Time);
					Time_Sum += (Vtx->Data[i].RTrans[j-1].Time);
				}
				Vtx->Data[i].Time = Time_Sum + (Vtx->Data[i].RTrans[j-1].Time);
			}
			else if((Vtx->Data[i].Status) == BRAM_WRITE)	// Read
			{
				Vtx->Data[i].WTrans[0].Start = (Vtx->Data[i].Start);

				Time_Sum = 0;
				for(j=1; j<NofWTrans; j++)
				{
					Vtx->Data[i].WTrans[j].Start = (Vtx->Data[i].WTrans[j-1].Start) + (Vtx->Data[i].WTrans[j-1].Time);
					Time_Sum += (Vtx->Data[i].WTrans[j-1].Time);
				}
				Vtx->Data[i].Time = Time_Sum + (Vtx->Data[i].WTrans[j-1].Time);				
			}	

			Free = 0;

			if(i<(NofData-1))
			{
				Free = (Vtx->Data[i].Start) + (Vtx->Data[i].Time) + (Vtx->Data[i].Addl_Time);
				Vtx->Data[i+1].Start = Free;
			}
		}
		int vertex_edge=0;

		for(i=0; i<NofData; i++)
		{
			if(vertex_edge < (Vtx->Data[i].Start + Vtx->Data[i].Time + Vtx->Data[i].Addl_Time))
				vertex_edge = (Vtx->Data[i].Start + Vtx->Data[i].Time + Vtx->Data[i].Addl_Time);
		}

		Vtx->Time = vertex_edge - (Vtx->Start);

	}
	else	//DMA
	{
		Vtx->Data[0].Start = Vtx->Start;

		for(i=0; i<NofData; i++)
		{
			int NofWTrans = Vtx->Data[i].NofWTrans;
			int NofRTrans = Vtx->Data[i].NofRTrans;

			int tmp = (Vtx->Data[i].NofBEAT)*DMA_TRANS_Cycle;
			int FIFO_Latency = min(BURST_SIZE, tmp);

			Vtx->Data[i].RTrans[0].Start = (Vtx->Data[i].Start) + DMA_SET;
			Time_SumR = 0;
			for(j=1; j<NofRTrans; j++)
			{
				Vtx->Data[i].RTrans[j].Start = (Vtx->Data[i].RTrans[j-1].Start) + (Vtx->Data[i].RTrans[j-1].Time);
				Time_SumR += (Vtx->Data[i].RTrans[j-1].Time);
			}
			Time_SumR += (Vtx->Data[i].RTrans[j-1].Time);

			Vtx->Data[i].WTrans[0].Start = Vtx->Data[i].RTrans[0].Start + FIFO_Latency;
			Time_SumW = 0;
			for(j=1; j<NofWTrans; j++)
			{				
				Vtx->Data[i].WTrans[j].Start = (Vtx->Data[i].WTrans[j-1].Start) + (Vtx->Data[i].WTrans[j-1].Time);
				Time_SumW += (Vtx->Data[i].WTrans[j-1].Time);				
			}			
			Time_SumW += (Vtx->Data[i].WTrans[j-1].Time);			

			if(((Vtx->Data[i].Status)==BRAM_WRITE) | ((Vtx->Data[i].Status)==BRAM_READ))
				Vtx->Data[i].Time = max((Time_SumW + FIFO_Latency), Time_SumR) + DMA_SET + (Vtx->Data[i].Addl_Time);
			else
				Vtx->Data[i].Time = max((Time_SumW + FIFO_Latency), Time_SumR) + (Vtx->Data[i].Addl_Time);
			
			if((Vtx->Data[i].Status) == BRAM_READ)		// READ
			{
				RFree = (Vtx->Data[i].Start) + (Vtx->Data[i].Time);
				WFree = (Vtx->Data[i].Start) + DMA_SET;

			}
			else if((Vtx->Data[i].Status) == BRAM_WRITE) // WRITE
			{
				RFree = (Vtx->Data[i].Start) + DMA_SET;
				WFree = (Vtx->Data[i].Start) + (Vtx->Data[i].Time);
			}
			else
			{
				RFree = (Vtx->Data[i].Start) + (Vtx->Data[i].Time);
				WFree = (Vtx->Data[i].Start) + (Vtx->Data[i].Time);
			}

			if(i<(NofData-1))
			{
				if((Vtx->Data[i+1].Status) == BRAM_READ)		// Write
				{
					Vtx->Data[i+1].Start = RFree;
				}
				else if((Vtx->Data[i+1].Status) == BRAM_WRITE)	// Read
				{
					Vtx->Data[i+1].Start = WFree;
				}
				else
				{
					Vtx->Data[i+1].Start = max(RFree, WFree);
				}
			}
			
		}
		
		int vertex_edge=0;

		for(i=0; i<NofData; i++)
		{
			if(vertex_edge < (Vtx->Data[i].Start + Vtx->Data[i].Time + Vtx->Data[i].Addl_Time))
				vertex_edge = (Vtx->Data[i].Start + Vtx->Data[i].Time + Vtx->Data[i].Addl_Time);
		}
		Vtx->Time = vertex_edge - (Vtx->Start);
	}

}

void RTransaction_offset(VERTEX *Vtx, int NData, int NTrans, int offset)
{
	int i         = 0;
	int j         = 0;
	int NofData   = Vtx->NofData;
	
	int ExecTime_R =0, ExecTime_W =0, ExecTime =0;
	int RFree      =0, WFree      =0, Free     =0;
	int Time_Sum   =0, Time_SumR  =0, Time_SumW=0;
	
	if(Vtx->Master == DMA)
	{
		for(i=NData; i<NofData; i++)
		{
			int NofRTrans = Vtx->Data[i].NofRTrans;
			int NofWTrans = Vtx->Data[i].NofWTrans;

			for(j= NTrans; j<NofRTrans; j++)		// DMA has same transaction in Read, write. NofRTrans can use in write transaction part.
			{
				Vtx->Data[i].RTrans[j].Start += offset;
			}
			for(j= NTrans; j<NofWTrans; j++)		// DMA has same transaction in Read, write. NofRTrans can use in write transaction part.
			{
				Vtx->Data[i].WTrans[j].Start += offset;
			}
			
		}
	}
	else
	{
		for(i=NData; i<NofData; i++)
		{
			int NofRTrans = Vtx->Data[i].NofRTrans;
			int NofWTrans = Vtx->Data[i].NofWTrans;

			for(j= NTrans; j<NofRTrans; j++)
			{
				Vtx->Data[i].RTrans[j].Start += offset;
			}
			for(j= NTrans; j<NofWTrans; j++)
			{
				Vtx->Data[i].WTrans[j].Start += offset;
			}

		}
	}
	
}

void WTransaction_offset(VERTEX *Vtx, int NData, int NTrans, int offset)
{
	int i         = 0;
	int j         = 0;
	int NofData   = Vtx->NofData;
	int NofRTrans = Vtx->Data[NData].NofRTrans;
	int NofWTrans = Vtx->Data[NData].NofWTrans;

	int ExecTime_R =0, ExecTime_W =0, ExecTime =0;
	int RFree      =0, WFree      =0, Free     =0;
	int Time_Sum   =0, Time_SumR  =0, Time_SumW=0;

	if(Vtx->Master == DMA)
	{
		for(i= NTrans; i<NofWTrans; i++)		// DMA has same transaction in Read, write. NofRTrans can use in write transaction part.
		{
			Vtx->Data[NData].WTrans[i].Start += offset;

		}
	}
	else
	{
		for(i= NTrans; i<NofWTrans; i++)		// DMA has same transaction in Read, write. NofRTrans can use in write transaction part.
		{
			Vtx->Data[NData].WTrans[i].Start += offset;
		}
	}

}

void vertex_shift(VERTEX *Vtx, int offset)
{
	int i,ii,iii;

	int NofData = Vtx->NofData;

	for(i=0; i<NofData; i++)
	{
		int NofRTrans = Vtx->Data[i].NofRTrans;
		int NofWTrans = Vtx->Data[i].NofWTrans;

		for(ii=0; ii<NofRTrans; ii++)
		{
			Vtx->Data[i].RTrans[ii].Start += offset;
		}
		for(ii=0; ii<NofWTrans; ii++)
		{
			Vtx->Data[i].WTrans[ii].Start += offset;
		}

		Vtx->Data[i].Start += offset;
	}
	Vtx->Start += offset;
}

void Print_Vertex(VERTEX Vtx)
{
	int NofDATA = Vtx.NofData;
	int p=0;
	int q=0;

	if(Vtx.Master == PS)
	{
		cout<<"PS "<<" Start: "<<Vtx.Start<<" Exec. Time: "<<Vtx.Time<<" End Time: "<<Vtx.Start+Vtx.Time<<endl;
		
		for(p=0; p<NofDATA; p++)
		{
			if((Vtx.Data[p].Status)==BRAM_READ)
			{
				//cout<<"   Read  Data "<<p<<" start: "<<Vtx.Data[p].Start<<" Time: "<<Vtx.Data[p].Time<<endl;
				int NofRTrans = Vtx.Data[p].NofRTrans;
				for(q=0; q<NofRTrans; q++)
				{
						//cout<<"RTrans "<<q<<" start: "<<Vtx.Data[p].RTrans[q].Start<<" Time: "<<Vtx.Data[p].RTrans[q].Time<<endl;
				}
			}
			else if((Vtx.Data[p].Status)==BRAM_WRITE)
			{
				//cout<<"   Write Data "<<p<<" start: "<<Vtx.Data[p].Start<<" Time: "<<Vtx.Data[p].Time<<endl;
				int NofRTrans = Vtx.Data[p].NofRTrans;
				for(q=0; q<NofRTrans; q++)
				{
					//	cout<<"RTrans "<<q<<" start: "<<Vtx.Data[p].RTrans[q].Start<<" Time: "<<Vtx.Data[p].RTrans[q].Time<<" WTrans "<<q<<" start: "<<Vtx.Data[p].WTrans[q].Start<<" Time: "<<Vtx.Data[p].WTrans[q].Time<<endl;
				}
			}
			else
			{
				//cout<<"    Compute   "<<p<<" start: "<<Vtx.Data[p].Start<<" Time: "<<Vtx.Data[p].Time<<endl;
				int NofRTrans = Vtx.Data[p].NofRTrans;
				for(q=0; q<NofRTrans; q++)
				{
					//	cout<<"RTrans "<<q<<" start: "<<Vtx.Data[p].RTrans[q].Start<<" Time: "<<Vtx.Data[p].RTrans[q].Time<<" WTrans "<<q<<" start: "<<Vtx.Data[p].WTrans[q].Start<<" Time: "<<Vtx.Data[p].WTrans[q].Time<<endl;
				}
			}
		}

	}
	else
	{
		cout<<"HW "<<" Start: "<<Vtx.Start<<" Exec. Time: "<<Vtx.Time<<" End Time: "<<Vtx.Start+Vtx.Time<<endl;

		for(p=0; p<NofDATA; p++)
		{
			if((Vtx.Data[p].Status)==BRAM_READ)
			{
				//cout<<"   Read  Data "<<p<<" start: "<<Vtx.Data[p].Start<<" Time: "<<Vtx.Data[p].Time<<endl;
				int NofRTrans = Vtx.Data[p].NofRTrans;
				for(q=0; q<NofRTrans; q++)
				{
					//	cout<<"RTrans "<<q<<" start: "<<Vtx.Data[p].RTrans[q].Start<<" Time: "<<Vtx.Data[p].RTrans[q].Time<<" WTrans "<<q<<" start: "<<Vtx.Data[p].WTrans[q].Start<<" Time: "<<Vtx.Data[p].WTrans[q].Time<<endl;
				}
			}
			else if((Vtx.Data[p].Status)==BRAM_WRITE)
			{
				//cout<<"   Write Data "<<p<<" start: "<<Vtx.Data[p].Start<<" Time: "<<Vtx.Data[p].Time<<endl;
				int NofRTrans = Vtx.Data[p].NofRTrans;
				for(q=0; q<NofRTrans; q++)
				{
					//	cout<<"RTrans "<<q<<" start: "<<Vtx.Data[p].RTrans[q].Start<<" Time: "<<Vtx.Data[p].RTrans[q].Time<<" WTrans "<<q<<" start: "<<Vtx.Data[p].WTrans[q].Start<<" Time: "<<Vtx.Data[p].WTrans[q].Time<<endl;
				}
			}
			else
			{
				//cout<<"    Compute   "<<p<<" start: "<<Vtx.Data[p].Start<<" Time: "<<Vtx.Data[p].Time<<endl;
				int NofRTrans = Vtx.Data[p].NofRTrans;
				for(q=0; q<NofRTrans; q++)
				{
						//cout<<"RTrans "<<q<<" start: "<<Vtx.Data[p].RTrans[q].Start<<" Time: "<<Vtx.Data[p].RTrans[q].Time<<" WTrans "<<q<<" start: "<<Vtx.Data[p].WTrans[q].Start<<" Time: "<<Vtx.Data[p].WTrans[q].Time<<endl;
				}
			}
			
		}

	}


}
