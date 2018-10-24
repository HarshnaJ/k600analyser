/* Sort code for HAGAR, the NaI gamma detector
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for gamma sort codes for K600 experiments.
 * 
 * 
 */

#include "GammaData.h"
#include "CloverSort.h"
#include <math.h>
#include <stdlib.h>
#include <TRandom3.h>
#include <vector>

extern int ADCModules;
extern int ADCsize;
extern int NumberOfClover;

extern int **CloverADCChannelLimits;
extern int **CloverTDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;

extern double **ADCCalibrationParameters;

extern double *GammaTimeOffsets;	        // from Parameters.c 
extern int *DetNrForGammaTimeOffsets;       // from Parameters.c  
extern int NrOfDetForGammaTimeOffsets;     // nr of Det for which we have GammaTimeoffsets read it via Parameters.c


TRandom3 *randy = new TRandom3(0);

int CountGammaHits[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
Double_t GammaTimeCloveroffset=0;

//GammaData *CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
void CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy)
{
  //GammaData *gammy = new GammaData();
  //Loop over ADC and TDC events and do the following:
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
  multiTDC *mTDC = new multiTDC;
  mTDC->multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);	
//printf("mTDC->GetSize(): %d\n",mTDC->GetSize());


  double *EnergyAddback = new double[NumberOfClover]; // Energy of the Addback
  int *EventAddback = new int[NumberOfClover]; // Event number in gammy in which the maximum energy deposition took place (per clover)
  //int *SegmentAddback = new int[NumberOfClover]; // Segment with maximum Energy deposit in the Clover (considered the first segment hit)
  double *MaxEnerAddback = new double[NumberOfClover]; // Maximum energy deposit in a segment for each clover
  int   *NSegmentsAddback = new int[NumberOfClover]; // Number of crystals contributing to the addback.
  int *MaxEnerAddbackSegm = new int[NumberOfClover]; //revised Segment with maximum Energy deposit in the Clover (considered the first segment hit)
  double *AddbackTime = new double[NumberOfClover]; // GammaTime of segment with first hit assigned as GammaTime of the Addback 

  for (int i=0; i<NumberOfClover;i++) 
  {
         EnergyAddback[i] = 0.;
         EventAddback[i] = -1;
       //  SegmentAddback[i] = -1;
         MaxEnerAddback[i] = 0.;
         NSegmentsAddback[i] = 0;
	 MaxEnerAddbackSegm[i]=-1;
	 AddbackTime[i]=0;
  }
  
  for(int k=0;k<mTDC->GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
   {CountGammaHits[0]++;
    if(CloverTDCTest(mTDC->GetChannel(k)))
      {CountGammaHits[1]++;
	int DetNum = CloverTDCIdentifyDetector(mTDC->GetChannel(k));
 // printf("+++++++++++++++++++DetNum %d\n",DetNum);
  int Segm = CloverTDCIdentifySegment(mTDC->GetChannel(k));
 // printf("-------------Segm %d\n",Segm);
	 if(DetNum>0)
		{	//printf("Test1\n");
		CountGammaHits[2]++;
		  for(int i=CloverADCChannelLimits[DetNum-1][0];i<=CloverADCChannelLimits[DetNum-1][1];i++)
		      {//printf("Test2\n");
		      CountGammaHits[3]++;
			if(CloverADCTDCChannelCheck(i,mTDC->GetChannel(k)))
			{//printf("Test3\n");
			CountGammaHits[4]++;
  			 // printf("line 55");
			  //printf("ADCChannel: %d \t TDCChannel: %d\n",i,mTDC->GetChannel(k));

	  			double GammaEnergy = CloverEnergyCalc(i,ADC_import[i]);
	  			if(GammaEnergy>0.1)
	  			{
	  			//printf("Test4\n");
	  			CountGammaHits[5]++;
	    			gammy->SetEnergy(GammaEnergy);
	    			
	    			// alignement of the GammaTime value+++++++++++++++
				GammaTimeCloveroffset =0;   // set it to zero, so that if nothing happens inside IF loop you have a value for it
				//printf("------------------NrOfDetForGammaTimeOffsets= %d \n",NrOfDetForGammaTimeOffsets); // as defined in Parameter.c 
				for (int n = 0; n< NrOfDetForGammaTimeOffsets;n++){
				       if( DetNrForGammaTimeOffsets[n] == (Segm+(DetNum-1)*4)) GammaTimeCloveroffset=GammaTimeOffsets[n];
			        //printf("------------------GammaTime offset= %f \n",GammaTimeOffsets[n]); // as defined in Parameter.c 
			        }
			      //  printf("Det %d Segm %d: GammaTime offset= %f \n",DetNum, Segm,GammaTimeCloveroffset);
			        //++++++++++++++++++
	    			
	    			gammy->SetTime(mTDC->GetValue(k)-GammaTimeCloveroffset);
				gammy->SetDetectorType("Clover");
				gammy->SetDetectorLabel(DetNum);
				gammy->SetDetectorSegm(Segm);
				gammy->SetGammaRawADC(ADC_import[i]);
				gammy->SetGammaADCChannel(i);
				gammy->SetGammaTDCChannel(mTDC->GetChannel(k));
				gammy->SetGammaTDCMultiplicity(mTDC->GetMult(k));
				


//Addback********************************************************************************************************** 				
				//*****************************Harshna

// At this point, should put criteria that if time-> (mTDC->GetValue(k)-GammaTimeCloveroffset) 
// between prompt peak range, then add to energy Addback?

				 //printf("Time in gammy: %d \t Time in ADC: %d \n",gammy->GetTime(k),mTDC->GetValue(k)-GammaTimeCloveroffset); 
				//printf("Time in ADC: %f \n",mTDC->GetValue(k)-GammaTimeCloveroffset); 
				 if((mTDC->GetValue(k)-GammaTimeCloveroffset)>=4300 && (mTDC->GetValue(k)-GammaTimeCloveroffset)<=4900)
				  {
				    EnergyAddback[DetNum-1] += GammaEnergy;
				    NSegmentsAddback[DetNum-1]++;
				
					if (GammaEnergy > MaxEnerAddback[DetNum-1])
					{
					  EventAddback[DetNum-1] = gammy->SizeOfEvent()-1;
					  MaxEnerAddback[DetNum-1] = GammaEnergy;
					  MaxEnerAddbackSegm[DetNum-1] = Segm;
					  AddbackTime[DetNum-1] = (mTDC->GetValue(k)-GammaTimeCloveroffset);
				        }
				  //printf("---0 gammy->SizeOfEvent: %d,  K value in loop: %d,\n", gammy->SizeOfEvent(), k);
 				  //printf("***1  Clover: %d Segm: %d \t Egam: %f \t Eaddback: %f \t  Segms fired: %d \t MaxDepSegm: %d \n ",DetNum, Segm, GammaEnergy, EnergyAddback[DetNum-1], NSegmentsAddback[DetNum-1], MaxEnerAddbackSegm[DetNum-1]);
  				  }


				//*****************************Harshna




				//EnergyAddback[DetNum-1] += GammaEnergy;
				//NSegmentsAddback[DetNum-1]++;
/*
				if (GammaEnergy > MaxEnerAddback[DetNum-1])
				{
				  EventAddback[DetNum-1] = gammy->SizeOfEvent()-1;
				  MaxEnerAddback[DetNum-1] = GammaEnergy;
				 
				  //printf("In. Size %d, Det %d, EvtAddBack%d,\n", gammy->SizeOfEvent(), DetNum, EventAddback[DetNum-1]);
				}
*/				

				/*
				printf("GetSize: %d;\n",gammy->SizeOfEvent());
				printf("EventN: %d; \n",EventAddback[DetNum-1]);
				printf("GammaEnergy: %f, gammyEnergy: %f \n",GammaEnergy,gammy->GetEnergy(EventAddback[DetNum-1]));
				printf("GammaEnergy: %f, gammyEnergy: %f \n",GammaEnergy,gammy->GetEnergy(gammy->SizeOfEvent()));
				printf("Segm: %d, gammySegm: %d \n", Segm, gammy->GetDetectorSegm(EventAddback[DetNum-1]));
				*/
	  			} 
			 }
      		       }
    		}
	}
    }
  

   
  for(int i=0;i<NumberOfClover;i++)
  {

    //if (EnergyAddback[i]>0) printf("##3  Clover No: %d \t Addback Energy: %f \t Max Energy: %f \t No. of segments fired: %d \n ",i+1, EnergyAddback[i], MaxEnerAddback[i], NSegmentsAddback[i]);


    if(EnergyAddback[i] > 0)
        {
//printf("##2  Clover No: %d \t Addback Energy: %f \t Max Energy: %f \t No. of segments fired: %d \n ",i+1, EnergyAddback[i], MaxEnerAddback[i], NSegmentsAddback[i]);

  //  if (EnergyAddback[i]>0) printf("##3  Clover No: %d \t Addback Energy: %f \t Max Energy: %f \t No. of segments fired: %d \n ",i+1, EnergyAddback[i], MaxEnerAddback[i], NSegmentsAddback[i]);

  	gammy->SetEnergy(EnergyAddback[i]);
  	gammy->SetDetectorType("Addback");
  	gammy->SetDetectorLabel(i+1);
  	// printf("gammy DetNum: %d, index DetNum: %d;\n",gammy->GetDetectorLabel(EventAddback[i]),i+1);
  	gammy->SetDetectorSegm(MaxEnerAddbackSegm[i]);
  	gammy->SetTime(AddbackTime[i]);
	gammy->SetGammaRawADC(gammy->GetGammaRawADC(EventAddback[i]));
	gammy->SetGammaADCChannel(gammy->GetGammaADCChannel(EventAddback[i]));
	gammy->SetGammaTDCChannel(gammy->GetGammaTDCChannel(EventAddback[i]));
	//gammy->SetGammaTDCMultiplicity(gammy->GetGammaTDCMultiplicity(EventAddback[i]));
	gammy->SetGammaTDCMultiplicity(NSegmentsAddback[i]);

    }
  }
  
	//for(int i=0;i<6;i++)printf("CountGammaHits[%d]: %d\n",i,CountGammaHits[i]);
  gammy->SetHits(gammy->SizeOfEvent());
  mTDC->ClearEvent();
  delete mTDC;
  
  delete EnergyAddback;
  delete MaxEnerAddback;
  //delete SegmentAddback;
  delete EventAddback;
  delete NSegmentsAddback;
  
  
  //return gammy;

//printf("\n Next event ********************************************************************* \n");
}



double CloverThetaCalc(int Channel)
{
  return 0;
}

double CloverPhiCalc(int Channel)
{
  return 0;
}

//function to identify the clover number
int CloverTDCIdentifyDetector(int TDCChannel)
{
 int testresult = -1;
 int result=0;
   for(int i=0;i<NumberOfClover;i++)
    {
     testresult = -1;
       if(TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])
       	{
	  		 testresult = i+1;
				}
			if(testresult>0) result=testresult;
//			printf("CloverTDCChannelLimits: %d  CloverTDCChannelLimits: %d \n TDC Channel: %d\n Clovernumber: %d\n ",CloverTDCChannelLimits[i][0], CloverTDCChannelLimits[i][1], TDCChannel,i);  
// 			printf("testresult = %d\n",testresult);
   }
//  printf("result = %d\n",result);
 return result;
}

//function to identify the segment number
int CloverTDCIdentifySegment(int TDCChannel)
{
 int testresult = -1;
 int result=0;
  for(int i=0;i<NumberOfClover;i++)
    {
      testresult = -1;
      if(TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])
	    {
	     testresult = (TDCChannel-CloverTDCChannelLimits[i][0])+1;
	    }
      if(testresult>0) result=testresult;
//			printf("CloverTDCChannelLimits: %d  CloverTDCChannelLimits: %d \n TDC Channel: %d\n Clovernumber: %d\n ",CloverTDCChannelLimits[i][0], CloverTDCChannelLimits[i][1], TDCChannel,i);  
 //			printf("testresult = %d\n",testresult);;
  }
 // printf("+++++++++++++result = %d\n",result);
  return result;
}

double CloverEnergyCalc(int Channel, double ADCValue)
{

		  
      double randNum = randy->Rndm();

  double RandyADCValue = ADCValue+randNum;

  int npars = ADCCalibrationParameters[Channel][0];

  double result = 0;

  for(int i=1;i<npars+1;i++)
  {
    result += ADCCalibrationParameters[Channel][i] * pow(RandyADCValue,(double)i-1.);
  }

  return result;
}

bool CloverTDCTest(int TDCChannel) // to check if the TDCChannel is the one associated with the Clovers
{
//printf("TDCChannel: %d\n",TDCChannel);
  bool result = false;
  for(int i=0;i<NumberOfClover;i++)
    {
      if(TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])
	{
	  result = true;
	}
      if(CloverTDCChannelLimits[i][0]==-1)result = true;
      if(CloverTDCChannelLimits[i][1]==-1)result = true;
    }
 	//if(result)printf("True\n");
 //	else printf("False\n");
  return result;
}


bool CloverADCTDCChannelCheck(int ADCChannel, int TDCChannel) // to consider only the TDC/ADC channel associated with a Clover
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfClover;i++)
  {
    if(ADCChannel>=CloverADCChannelLimits[i][0] && ADCChannel<=CloverADCChannelLimits[i][1] && TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
   {
     if(ADCChannel-CloverADCChannelLimits[i][0]==TDCChannel-CloverTDCChannelLimits[i][0])
     {
     //   printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(CloverADCChannelLimits[i][0]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(CloverADCChannelLimits[i][1]==-1)result = true;
     if(CloverTDCChannelLimits[i][0]==-1)result = true;
     if(CloverTDCChannelLimits[i][1]==-1)result = true;
     }
  return result;
}
/*
int CloverDetHitNumber(int ADCChannel) //number of hit in one Clover
{
  int result = 0;
  for(int i=0;i<NumberOfClover;i++)
  {
    if(ADCChannel>=CloverADCChannelLimits[i][0] && ADCChannel<=CloverADCChannelLimits[i][1])
      {
	result = i+1;
      }
  }
printf("result = %d\n",result);
  return result;
}
*/
