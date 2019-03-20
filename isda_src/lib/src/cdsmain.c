/*
 * ISDA CDS Standard Model
 *
 * Copyright (C) 2009 International Swaps and Derivatives Association, Inc.
 * Developed and supported in collaboration with Markit
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the ISDA CDS Standard Model Public License.
 */

#include "cerror.h"
#include "tcurve.h"
#include "cdsone.h"
#include "convert.h"
#include "zerocurve.h"
#include "cds.h"
#include "cxzerocurve.h"
#include "dateconv.h"
#include "date_sup.h"
#include "busday.h"
#include "ldate.h"




int __declspec(dllexport) hell()
{
    printf("inCDSMAINC");

    return 0;
}
/*
***************************************************************************
** Build IR zero curve in c
***************************************************************************
*/
/* TCurve is Defined in bashtypes .h
    typedef struct _TCurve
        {
            int       fNumItems;     //Number of TRatePts in fArray 
            TRatePt  *fArray;        // Dates & rates 
            TDate     fBaseDate;     // Discount date  
            double    fBasis;        // Number compounding periods / year 
            long      fDayCountConv; // How the year fraction is computed 
TCurve BuildExampleZeroCurve(double rates[], char *expiries[], char *types)
        typedef struct
        {
            TDate  fDate;
            double fRate;
        } TRatePt;
*/
TCurve __declspec(dllexport) *BuildExampleZeroCurve(double rates[], char *expiries[], char *types)
{
    static char *routine = "BuildExampleZeroCurve";
    TCurve *zc = NULL;
    //char *types = "MMMMMSSSSSSSSS";
    //char *types = "MMMMMMSSSSSSSSSSSSSSS";

    //char *expiries[14] = {"1M", "2M", "3M", "6M", "9M", "1Y", "2Y", "3Y", "4Y", "5Y", "6Y", "7Y", "8Y", "9Y"};
    /*char *expiries[21] = {
        "1M",
        "2M",
        "3M",
        "6M",
        "9M",
        "1Y",
        "2Y",
        "3Y",
        "4Y",
        "5Y",
        "6Y",
        "7Y",
        "8Y",
        "9Y",
        "10Y",
        "11Y",
        "12Y",
        "15Y",
        "20Y",
        "25Y",
        "30Y"};*/
    TDate *dates = NULL;
    //rates value 1e-9 is 0.000000001
    //double rates[14] = {1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9};
    /*double rates[14] = {0.023009, 0.024521, 0.026445, 0.028626,
                        0.031236, 0.030055, 0.030420, 0.030620,
                        0.030320, 0.030500, 0.030635, 0.030920,
                        0.031130, 0.031325}; 0.031725, 0.032075,
                        0.32315, 0.32275, 0.32215};*/
    /*double rates[21] = {0.004450, 0.009488, 0.012337, 0.017762,
                        0.019350, 0.020838, 0.016520, 0.020180,
                        0.023033, 0.025250, 0.026960, 0.028250,
                        0.029310, 0.030170, 0.030920, 0.031600,
                        0.032310, 0.033670, 0.034190, 0.034110, 0.034120};*/
    TDate baseDate;
    long mmDCC;
    TDateInterval ivl;
    long dcc;
    double freq;
    char badDayConv = 'M';
    char *holidays = "None";
    int i, n;

    //printf("Step 1: Calling the jpmcdsDate function\n");
    //baseDate = JpmcdsDate(2008, 1, 3);
    baseDate = JpmcdsDate(2007, 3, 20);
    // Create value for mmDCC required for building JpmcdsBuildIRZeroCurve function call
    if (JpmcdsStringToDayCountConv("Act/360", &mmDCC) != SUCCESS)
        goto done;
    //printf("Step2: The value of mmDCC is %ld \n", mmDCC);

    // Create value for dcc required for building JpmcdsBuildIRZeroCurve function call
    if (JpmcdsStringToDayCountConv("30/360", &dcc) != SUCCESS)
        goto done;
    //printf("Step3: The value of dcc is %ld \n", dcc);

    if (JpmcdsStringToDateInterval("6M", routine, &ivl) != SUCCESS)
        goto done;
    //printf("Step4:The value of interval period is %d \n", ivl.prd);
    //printf("Step5: The value of interval period type is %c \n", ivl.prd_typ);
    //printf("Step6: The value of interval flag is is %d \n", ivl.flag);
    // Create value for frequency freq required for building JpmcdsBuildIRZeroCurve function call
    if (JpmcdsDateIntervalToFreq(&ivl, &freq) != SUCCESS)
        goto done;
    //printf("Step7: The value of frequency is %e \n", freq);
    //printf("###################################################################\n");

    n = strlen(types);
    //printf("Step8: The value of n is %d \n", n);

    dates = NEW_ARRAY(TDate, n);
    //printf(" -------| ------------| ----- |-------------------\n");
    //printf(" period | Period Type | Dates |JPMCDS_BAD_DAY_NONE\n");
    //printf(" -------| ------------| ----- |-------------------\n");
    for (i = 0; i < n; i++)
    {
        TDateInterval tmp;

        if (JpmcdsStringToDateInterval(expiries[i], routine, &tmp) != SUCCESS)
        {
            JpmcdsErrMsg("%s: invalid interval for element[%d].\n", routine, i);
            goto done;
        }

        //printf("##### the JPMCDS_BAD_DAY_NONE is %ld \n", JPMCDS_BAD_DAY_NONE);
        if (JpmcdsDateFwdThenAdjust(baseDate, &tmp, JPMCDS_BAD_DAY_NONE, "None", dates + i) != SUCCESS)
        {
            JpmcdsErrMsg("%s: invalid interval for element[%d].\n", routine, i);
            goto done;
        }
        //printf(" %d     | %c          | %ld | %ld \n", tmp.prd, tmp.prd_typ, dates[i], JPMCDS_BAD_DAY_NONE);
        //printf("Dates + I is %ld \n", *dates + i);
    }
    //printf("###################################################################\n");
    //printf("Step9: calling JpmcdsBuildIRZeroCurve function and returned with\n");
    zc = JpmcdsBuildIRZeroCurve(
        baseDate,
        types,
        dates,
        rates,
        n,
        mmDCC,
        (long)freq,
        (long)freq,
        dcc,
        dcc,
        badDayConv,
        holidays);
done:
    FREE(dates);

    //printf("###################################################################\n");
    //printf("Step23: DISCOUNT CURVE or Finalized Zero curve \n");
    //printf("----------- |--------------- | --- | --------- | ---------- | ----------- \n");
    //printf("Basis value |Number of items | DCC | Base Date | Array Rate | Array Date \n");
    //printf("----------- |--------------- | --- | --------- | ---------- | ----------- \n");
    /*for (i = 0; i < zc->fNumItems; i++)
    {
        //printf("##### The zerocurve Basis value is %f \n", zc->fBasis);
        //printf("##### The zerocurve Number of items %d \n", zc->fNumItems);
        //printf("##### The zerocurve DCC %ld \n", zc->fDayCountConv);
        //printf("##### The zerocurve Discount date cached number is %ld \n", zc->fBaseDate);
        //printf("##### The zerocurve Array Rate %f \n", zc->fArray[i].fRate);
        //printf("##### The zerocurve Array Date %ld \n", zc->fArray[i].fDate);

        printf("%.9f    | %d             | %ld   | %ld    | %.9f   | %ld \n",
               zc->fBasis,
               zc->fNumItems,
               zc->fDayCountConv,
               zc->fBaseDate,
               zc->fArray[i].fRate,
               zc->fArray[i].fDate);
    }*/
    return zc;
}

/*
***************************************************************************
** Calculate upfront charge in c
***************************************************************************
*/
double __declspec(dllexport) CalcUpfrontCharge(TCurve *curve, double couponRate, double notional)
{
    static char *routine = "CalcUpfrontCharge";
    TDate today;
    TDate valueDate;
    TDate startDate;
    TDate benchmarkStart;
    TDate stepinDate;
    TDate endDate;
    TBoolean payAccOnDefault = TRUE;
    TDateInterval ivl;
    TStubMethod stub;
    long dcc;
    double parSpread = 550; //3600
    double recoveryRate = 0.4;
    TBoolean isPriceClean = TRUE; //originally FALSE in JPMC code
    //double notional = 1e7;
    double result = -1.0;

    if (curve == NULL)
    {
        JpmcdsErrMsg("CalcUpfrontCharge: NULL IR zero curve passed\n");
        goto done;
    }
    //printf("###################################################################\n");
    //printf("Step24: UPFRONT CHARGE CALCULATION FILE main.C \n");
    //printf("###################################################################\n");
    //today = JpmcdsDate(2008, 2, 1);
    today = JpmcdsDate(2008, 9, 18);
    //printf("Step25: Create cached entry for TODAY date 02/01/2008 %ld \n", today);
    //valueDate = JpmcdsDate(2008, 2, 1);
    valueDate = JpmcdsDate(2008, 9, 21);
    //printf("Step26: Create cached entry for VALUE date 02/01/2008 %ld \n", valueDate);
    //benchmarkStart = JpmcdsDate(2008, 2, 2);
    benchmarkStart = JpmcdsDate(2008, 7, 20);
    //printf("Step27: Create cached entry for BenchmarkStart date 02/02/2008 %ld \n", benchmarkStart);
    //startDate = JpmcdsDate(2008, 2, 2);
    startDate = JpmcdsDate(2007, 3, 20);
    //printf("Step28: Create cached entry for Start date 02/08/2008 %ld \n", startDate);
    //endDate = JpmcdsDate(2008, 2, 12);
    endDate = JpmcdsDate(2013, 6, 20);
    //printf("Step29: Create cached entry for End date 02/12/2008 %ld \n", endDate);
    //stepinDate = JpmcdsDate(2008, 2, 9);
    stepinDate = JpmcdsDate(2008, 9, 19);
    //printf("Step30: Create cached entry for Stepin date 02/09/2008 %ld \n", stepinDate);

    if (JpmcdsStringToDayCountConv("Act/360", &dcc) != SUCCESS)
        goto done;
    //printf("Step31: Act/360 String to day count %ld \n", dcc);

    if (JpmcdsStringToDateInterval("Q", routine, &ivl) != SUCCESS)
        goto done;
    //printf("Step32: The value of interval 1S Semi annual period is %d \n", ivl.prd);
    //printf("Step33: The value of interval 1S Semi annual period type is %c \n", ivl.prd_typ);
    //printf("Step34: The value of interval 1S Semi annual flag is %d \n", ivl.flag);
    if (JpmcdsStringToStubMethod("f/s", &stub) != SUCCESS)
        goto done;
    //printf("Step35: The value of f/s stub at end is %s \n", stub.stubAtEnd ? "true" : "false");
    //printf("Step36: The value of f/s longstub is %s \n", stub.longStub ? "true" : "false");
    if (JpmcdsCdsoneUpfrontCharge(today,
                                  valueDate,
                                  benchmarkStart,
                                  stepinDate,
                                  startDate,
                                  endDate,
                                  couponRate / 10000.0,
                                  payAccOnDefault,
                                  &ivl,
                                  &stub,
                                  dcc,
                                  'F',
                                  "None",
                                  curve,
                                  parSpread / 10000.0,
                                  recoveryRate,
                                  isPriceClean,
                                  &result) != SUCCESS)

        goto done;
done:

    return result * notional;
}

/*
***************************************************************************
** Calculate upfront basis points in c
***************************************************************************
*/
double __declspec(dllexport) CalcUpfrontBasisPoints(TCurve *curve, double couponRate)
{
    static char *routine = "CalcUpfrontCharge";
    TDate today;
    TDate valueDate;
    TDate startDate;
    TDate benchmarkStart;
    TDate stepinDate;
    TDate endDate;
    TBoolean payAccOnDefault = TRUE;
    TDateInterval ivl;
    TStubMethod stub;
    long dcc;
    double parSpread = 550; //3600
    double recoveryRate = 0.4;
    TBoolean isPriceClean = TRUE; //originally FALSE in JPMC code
    double result = -1.0;

    if (curve == NULL)
    {
        JpmcdsErrMsg("CalcUpfrontCharge: NULL IR zero curve passed\n");
        goto done;
    }

    //today = JpmcdsDate(2008, 2, 1);
    today = JpmcdsDate(2008, 9, 18);
    //printf("Step25: Create cached entry for TODAY date 02/01/2008 %ld \n", today);
    //valueDate = JpmcdsDate(2008, 2, 1);
    valueDate = JpmcdsDate(2008, 9, 21);
    //printf("Step26: Create cached entry for VALUE date 02/01/2008 %ld \n", valueDate);
    //benchmarkStart = JpmcdsDate(2008, 2, 2);
    benchmarkStart = JpmcdsDate(2008, 7, 20);
    //printf("Step27: Create cached entry for BenchmarkStart date 02/02/2008 %ld \n", benchmarkStart);
    //startDate = JpmcdsDate(2008, 2, 2);
    startDate = JpmcdsDate(2007, 3, 20);
    //printf("Step28: Create cached entry for Start date 02/08/2008 %ld \n", startDate);
    //endDate = JpmcdsDate(2008, 2, 12);
    //Maturity Date
    endDate = JpmcdsDate(2013, 6, 20);
    //printf("Step29: Create cached entry for End date 02/12/2008 %ld \n", endDate);
    //stepinDate = JpmcdsDate(2008, 2, 9);
    stepinDate = JpmcdsDate(2008, 9, 19);
    //printf("Step30: Create cached entry for Stepin date 02/09/2008 %ld \n", stepinDate);

    if (JpmcdsStringToDayCountConv("Act/360", &dcc) != SUCCESS)
        goto done;
    //printf("Step31: Act/360 String to day count %ld \n", dcc);

    if (JpmcdsStringToDateInterval("Q", routine, &ivl) != SUCCESS)
        goto done;
    //printf("Step32: The value of interval 1S Semi annual period is %d \n", ivl.prd);
    //printf("Step33: The value of interval 1S Semi annual period type is %c \n", ivl.prd_typ);
    //printf("Step34: The value of interval 1S Semi annual flag is %d \n", ivl.flag);
    if (JpmcdsStringToStubMethod("f/s", &stub) != SUCCESS)
        goto done;
    //printf("Step35: The value of f/s stub at end is %s \n", stub.stubAtEnd ? "true" : "false");
    //printf("Step36: The value of f/s longstub is %s \n", stub.longStub ? "true" : "false");
    //Calculate the basis points
    if (JpmcdsCdsoneUpfrontCharge(today,
                                  valueDate,
                                  benchmarkStart,
                                  stepinDate,
                                  startDate,
                                  endDate,
                                  couponRate / 10000.0,
                                  payAccOnDefault,
                                  &ivl,
                                  &stub,
                                  dcc,
                                  'F',
                                  "None",
                                  curve,
                                  parSpread / 10000.0,
                                  recoveryRate,
                                  isPriceClean,
                                  &result) != SUCCESS)

        goto done;
done:

    return result;
}




/*
#################################################
JpmcdsCdsoneSpread
Calculate the par spread in c
##################################################
*/
double __declspec(dllexport) CalcJpmcdsCdsoneSpread(TCurve *curve,
                              double couponRate,
                              double upfrontbasispoints,
                              int todayYear,
                              int todayMonth,
                              int todayDate)
{
    static char *routine = "JpmcdsCdsoneSpread";
    TDate today;
    TDate valueDate;
    TDate startDate;
    TDate benchmarkStart;
    TDate stepinDate;
    TDate endDate;
    TBoolean payAccOnDefault = TRUE;
    TDateInterval ivl;
    TStubMethod stub;
    long dcc;
    double recoveryRate = 0.4;
    TBoolean isPriceClean = TRUE; //originally FALSE in JPMC code
    double result = -1.0;

    if (curve == NULL)
    {
        JpmcdsErrMsg("CalcUpfrontCharge: NULL IR zero curve passed\n");
        goto done;
    }

    //today = JpmcdsDate(2008, 2, 1);
    today = JpmcdsDate(todayYear, todayMonth, todayDate);
    //printf("Step25: Create cached entry for TODAY date 02/01/2008 %ld \n", today);
    //valueDate = JpmcdsDate(2008, 2, 1);
    valueDate = JpmcdsDate(2018, 7, 31);
    //printf("Step26: Create cached entry for VALUE date 02/01/2008 %ld \n", valueDate);
    //benchmarkStart = JpmcdsDate(2008, 2, 2);
    benchmarkStart = JpmcdsDate(2017, 3, 20);
    //printf("Step27: Create cached entry for BenchmarkStart date 02/02/2008 %ld \n", benchmarkStart);
    //startDate = JpmcdsDate(2008, 2, 2);
    startDate = JpmcdsDate(2017, 3, 20);
    //printf("Step28: Create cached entry for Start date 02/08/2008 %ld \n", startDate);
    //endDate = JpmcdsDate(2008, 2, 12);
    //Maturity Date
    endDate = JpmcdsDate(2023, 6, 20);
    //printf("Step29: Create cached entry for End date 02/12/2008 %ld \n", endDate);
    //stepinDate = JpmcdsDate(2008, 2, 9);
    stepinDate = JpmcdsDate(2018, 8, 1);
    //printf("Step30: Create cached entry for Stepin date 02/09/2008 %ld \n", stepinDate);

    if (JpmcdsStringToDayCountConv("Act/360", &dcc) != SUCCESS)
        goto done;
    //printf("Step31: Act/360 String to day count %ld \n", dcc);

    if (JpmcdsStringToDateInterval("Q", routine, &ivl) != SUCCESS)
        goto done;
    //printf("Step32: The value of interval 1S Semi annual period is %d \n", ivl.prd);
    //printf("Step33: The value of interval 1S Semi annual period type is %c \n", ivl.prd_typ);
    //printf("Step34: The value of interval 1S Semi annual flag is %d \n", ivl.flag);
    if (JpmcdsStringToStubMethod("f/s", &stub) != SUCCESS)
        goto done;
    //printf("Step35: The value of f/s stub at end is %s \n", stub.stubAtEnd ? "true" : "false");
    //printf("Step36: The value of f/s longstub is %s \n", stub.longStub ? "true" : "false");
    //Calculate the par spread by providing the upfront basis points
    if (JpmcdsCdsoneSpread(today,
                           valueDate,
                           benchmarkStart,
                           stepinDate,
                           startDate,
                           endDate,
                           (couponRate / 10000.0), //pass the coupon rate
                           payAccOnDefault,
                           &ivl,
                           &stub,
                           dcc,
                           'F',
                           "None",
                           curve,
                           upfrontbasispoints, //pass the upfront basis point
                           recoveryRate,
                           isPriceClean,
                           &result) != SUCCESS)

        goto done;
done:
    //printf("Results in c is %f\n", result);
    return result;
}
/*
***************************************************************************
** NOTE: PYTHON ENTRY POINT
** call this function from python to calculate upfront basis points
***************************************************************************
*/

double __declspec(dllexport) py_calculate_cds_upfront_basis_points(int couponRate, double rates[], char *expiries[], char *types)
{
    double upfrontbasispoints = -1.0;
    TCurve *zerocurve = NULL;

    zerocurve = BuildExampleZeroCurve(rates, expiries, types);
    upfrontbasispoints = CalcUpfrontBasisPoints(zerocurve, couponRate);
    //printf("Upfront charge  in c    =  %f\n", upfrontbasispoints);
    return upfrontbasispoints;
}


/*
***************************************************************************
** NOTE: PYTHON ENTRY POINT
** call this function from python to calculate par spread
***************************************************************************
*/

double __declspec(dllexport) py_calculate_cds_par_spread(int couponRate,
                                   double rates[],
                                   char *expiries[],
                                   char *types,
                                   double upfrontbasispoints,
                                   int todayYear,
                                   int todayMonth,
                                   int todayDate)
{
    double parSpread = -1.0;
    TCurve *zerocurve = NULL;

    zerocurve = BuildExampleZeroCurve(rates, expiries, types);
    parSpread = CalcJpmcdsCdsoneSpread(zerocurve,
                                       couponRate,
                                       upfrontbasispoints,
                                       todayYear,
                                       todayMonth,
                                       todayDate);
    //printf("Upfront charge  in c    =  %f\n", upfrontbasispoints);
    return parSpread;
}
