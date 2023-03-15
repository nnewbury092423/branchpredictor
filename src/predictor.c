//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID = "PID";
const char *email = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;
int state;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
// TODO: Add your own Branch Predictor data structures here
int history;
int *ght;
int *pht;
int *btb;
int *chooser;
int *p1pht;
int *p2pht;
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_predictor()
{
    if (bpType == CUSTOM)
  {
    pcIndexBits = 13;
    ghistoryBits = 13;
  }


  // allocate memory for array
  ght = (int *)malloc(sizeof(int) * 1 << ghistoryBits);
  // initialize as all true
  for (int i = 0; i < 1 << ghistoryBits; i++)
  {
    ght[i] = 1;
  }

  pht = (int *)malloc(sizeof(int) * 1 << pcIndexBits);
  for (int i = 0; i < 1 << pcIndexBits; i++)
  {
    pht[i] = 1;
  }

  // initialize as all true
  // for(int i=0;i<1<<pcIndexBits;i++)
  //{
  // pht[i] = 1;
  //}

  btb = (int *)malloc(sizeof(int) * 1 << lhistoryBits);
  // initialize as all true
  for (int i = 0; i < 1 << lhistoryBits; i++)
  {
    btb[i] = 1;
  }
  if (bpType == TOURNAMENT)
  {
  chooser = (int *)malloc(sizeof(int) * 1 << ghistoryBits);
  // initialize as all true
  for (int i = 0; i < 1 << ghistoryBits; i++)
  {
    chooser[i] = 1;
  }
  }
  else
  {
    chooser = (int *)malloc(sizeof(int) * 1 << pcIndexBits);
  // initialize as all true
  for (int i = 0; i < 1 << pcIndexBits; i++)
  {
    chooser[i] = 1;
  }
  }
  p1pht = (int *)malloc(sizeof(int) * 1 << ghistoryBits);
  for (int i = 0; i < 1 << ghistoryBits; i++)
  {
      p1pht [i] = 1;
  }

  p2pht = (int *)malloc(sizeof(int) * 1 << ghistoryBits);
  for (int i = 0; i < 1 << ghistoryBits; i++)
  {
     p2pht [i] = 1;
  }



  //
  // TODO: Initialize Branch Predictor Data Structures
  // ghtsize = ghistoryBits;
  // lhtsize = lhistoryBits;

  // struct btb bup;
  //
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  // TODO: Implement prediction scheme
  // printf(" state: %d \n", state);
  // int onb = stateinterpret();
  // printf("onb %d \n", onb);

  // Make a prediction based on the bpType
  int address;
  int gpredict;
  int lpredict;
  int pclim;
  int pattern;
  int p1predict;
  int p2predict;

  switch (bpType)
  {
  case STATIC:
    return NOTTAKEN;
    break;
  case GSHARE:

    address = pc ^ history;
    address = address & ((1 << ghistoryBits) - 1);
    if (ght[address] < 2)
    {
      return NOTTAKEN;
    }
    else
    {
      return TAKEN;
    }

    break;
  case TOURNAMENT:
    // global
    address = pc ^ history;
    address = address & ((1 << ghistoryBits) - 1);
    if (ght[address] < 2)
    {
      gpredict = 0;
    }
    else
    {
      gpredict = 1;
    }

    // local
    pclim = pc & ((1 << pcIndexBits) - 1);
    pattern = pht[pclim];
    if (btb[pattern] < 2)
    {
      lpredict = 0;
    }
    else
    {
      lpredict = 1;
    }

    // chooser
    if (chooser[address] < 2)
    {
      return lpredict;
    }
    else
    {
      return gpredict;
    }
    break;
  case CUSTOM:

    pclim = pc & ((1 << pcIndexBits) - 1);
    address = pc ^ history;
    address = address & ((1 << ghistoryBits) - 1);

    if (p1pht[address] < 2)
    {
      p1predict = 0;
    }
    else
    {
      p1predict = 1;
    }

    // local
    if (p2pht[address] < 2)
    {
      p2predict = 0;
    }
    else
    {
      p2predict = 1;
    }

    if (chooser[pclim] < 2)
    {
      return p1predict;
    }
    else
    {
      return p2predict;
    }

  // return onb;
  default:

    break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_predictor(uint32_t pc, uint8_t outcome)
{

  switch (bpType)
  {
    // fill out gshare
    int address;
    int pclim;
    int pattern;
    int btbindex;
    int gpredict;
    int lpredict;
    int choice;
    int p1predict;
    int p2predict;
    int pred;

  case GSHARE:
    address = pc ^ history;
    address = address & ((1 << ghistoryBits) - 1);
    // printf("pc: %d \n",address );
    //printf("outcome: %d \n",outcome );
    //printf("ght[address]: %d \n",ght[address] );
    ght[address] = ght[address] + 2 * outcome - 1;
    if (ght[address] == 4)
    {
      ght[address]--;
    }
    else if (ght[address] == -1)
    {
      ght[address]++;
    }
    history = (history << 1) + outcome;
    //history = history & ((1 << ghistoryBits) - 1);
    //printf("outcome: %d \n",outcome );
    //printf("ght[address] after : %d \n",ght[address] );
    break;

  case TOURNAMENT:
    // get addresses
    address = pc ^ history;
    address = address & ((1 << ghistoryBits) - 1);
    pclim = pc & ((1 << pcIndexBits) - 1);
    btbindex = pht[pclim];

    // find outcome
    if (btb[btbindex] < 2)
    {
      lpredict = NOTTAKEN;
    }
    else
    {
      lpredict = TAKEN;
    }

    if (ght[address] < 2)
    {
      gpredict = NOTTAKEN;
    }
    else
    {
      gpredict = TAKEN;
    }

    // printf("pc: %d \n",address );
    ght[address] = ght[address] + 2 * outcome - 1;
    if (ght[address] == 4)
    {
      ght[address]--;
    }
    else if (ght[address] == -1)
    {
      ght[address]++;
    }

    // fill out pht
    // at pc address -> add

    // fill out btb

    btb[btbindex] = btb[btbindex] + 2 * outcome - 1;
    if (btb[btbindex] == 4)
    {
      btb[btbindex]--;
    }
    else if (btb[btbindex] == -1)
    {
      btb[btbindex]++;
    }

    // lpredict -
    // printf("outcome : %d \n", outcome);
    // printf("lpredict : %d \n", lpredict);
    // printf("gpredict : %d \n", gpredict);
    // printf("first : %d \n", chooser[address]);

    // chooser
    choice =   ( 1-outcome ^ gpredict) -  (1 - outcome ^ lpredict) ;
    chooser[address] = chooser[address] + choice;
    // printf("inc  : %d \n", (outcome^gpredict - outcome^lpredict));
    // printf("save : %d \n", chooser[address]);
    if (chooser[address] == 4)
    {
      chooser[address]--;
    }
    else if (chooser[address] == -1)
    {
      chooser[address]++;
    }

    // update global history
    history = (history << 1) + outcome;
    history = history & ((1 << ghistoryBits) - 1);

    // update pht
    //pattern = (btbindex<<1) + outcome;
    pht[pclim] = (btbindex << 1) + outcome;
    pht[pclim] = pht[pclim] & ((1 << lhistoryBits) - 1);

    break;
  case CUSTOM: //- bimode predictor

    // split global history table
    // choice table
    address = pc ^ history;
    address = address & ((1 << ghistoryBits) - 1);
    pclim = pc & ((1 << pcIndexBits) - 1);
    // make predictions

    if (p1pht[address] < 2)
    {
      p1predict = 0;
    }
    else
    {
      p1predict = 1;
    }

    // local
    if (p2pht[address] < 2)
    {
      p2predict = 0;
    }
    else
    {
      p2predict = 1;
    }

    if (chooser[pclim] < 2)
    {
      pred = p1predict;
      choice = 0;
    }
    else
    {
      pred = p2predict;
      choice = 1;
    }

    // fill out phts
    if (chooser[pclim] < 2)
    {
      p1pht[address] = p1pht[address] + 2 * outcome - 1;
      if (p1pht[address] == 4)
      {
        p1pht[address]--;
      }
      else if (p1pht[address] == -1)
      {
        p1pht[address]++;
      }
    }
    else
    {
      p2pht[address] = p2pht[address] + 2 * outcome - 1;
      if (p2pht[address] == 4)
      {
        p2pht[address]--;
      }
      else if (p2pht[address] == -1)
      {
        p2pht[address]++;
      }
    }

    // update choice predictor
    if (pred == outcome & choice != outcome)
    {
    }
    else
    {
      chooser[address] = chooser[address]+ 2 * outcome - 1;

      if (chooser[address] == 6)
      {
        chooser[address]--;
      }
      else if (chooser[address] == -3)
      {
        chooser[address]++;
      }
    }

    // update global history
    history = history << 1 + outcome;
    history = history & ((1 << ghistoryBits) - 1);

     return;
  }

  // printf(" outcome: %d \n", outcome);
  //
  // TODO: Implement Predictor training
  //  update thing in struture

  // printf(" state: %d \n", state);

  // printf(" temp: %d \n", temp);

  // printf("pc: %d", pc);
  //
}
