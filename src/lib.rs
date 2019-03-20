#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use libc::{c_double, c_long};
use std::ffi::CString;
use std::ops::DerefMut;
use std::os::raw;

const Success: i32 = SUCCESS as i32; // 0
const _Failure: i32 = FAILURE as i32; // -1

pub fn CalcUpfrontCharge(mut zero_curve: Box<_TCurve>, coupon_rate: libc::c_double) -> f64 {
    let routine = CString::new("CalcUpfrontCharge").expect("Could not build routine");

    let today = unsafe { JpmcdsDate(2019, 1, 17) };
    let valueDate = unsafe { JpmcdsDate(2019, 1, 22) };
    let benchmarkStart = unsafe { JpmcdsDate(2019, 1, 17) };
    let startDate = unsafe { JpmcdsDate(2018, 1, 17) };
    let endDate = unsafe { JpmcdsDate(2023, 12, 20) };
    let stepinDate = unsafe { JpmcdsDate(2019, 1, 18) };

    let par_spread = 500.0;
    let recovery_rate = 0.4;

    let dcc = unsafe {
        let mut dcc: c_long = 0;

        if JpmcdsStringToDayCountConv(
            CString::new("Act/360").expect("").into_raw(),
            &mut dcc as *mut _,
        ) != Success
        {
            panic!()
        };
        dcc
    };

    let mut ivl = unsafe {
        let mut ivl = TDateInterval {
            prd: 0 as raw::c_int,
            prd_typ: ' ' as raw::c_char,
            flag: 0 as raw::c_int,
        };

        if JpmcdsStringToDateInterval(
            CString::new("Q").expect("").into_raw(),
            routine.clone().into_raw(),
            &mut ivl as *mut _,
        ) != Success
        {
            panic!();
        };
        ivl
    };

    let mut stub = unsafe {
        let mut stub = _TStubMethod {
            stubAtEnd: 0 as raw::c_int,
            longStub: 0 as raw::c_int,
        };
        if JpmcdsStringToStubMethod(
            CString::new("F/S").expect("").into_raw(),
            &mut stub as *mut _,
        ) != Success
        {
            panic!()
        };
        stub
    };

    let is_price_clean: i32 = TRUE as i32;
    let pay_acc_on_default: i32 = TRUE as i32;

    let (upfront_charge, is_ok): (f64, i32) = unsafe {
        let mut upfront_charge: c_double = 0.0;
        let mut zc = *zero_curve.deref_mut();

        let is_ok = JpmcdsCdsoneUpfrontCharge(
            today,
            valueDate,
            benchmarkStart,
            stepinDate,
            startDate,
            endDate,
            coupon_rate / 10000.0,
            pay_acc_on_default,
            &mut ivl as *mut _,
            &mut stub as *mut _,
            dcc,
            'F' as i64,
            CString::new("None").expect("").into_raw(),
            &mut zc as *mut _,
            par_spread / 10000.0,
            recovery_rate,
            is_price_clean,
            &mut upfront_charge as *mut _,
        );
        println! {"Upfront charge!{:?}", upfront_charge};

        (upfront_charge, is_ok)
    };

    if is_ok != Success {
        panic!()
    };
    upfront_charge.clone()
}

pub fn BuildZeroCurve(mut rates: Vec<c_double>, expires: Vec<&str>, types: &str) -> Box<_TCurve> {
    let routine = CString::new("BuildZeroCurve").expect("Could not build routine");

    let mmDCC: c_long = unsafe {
        let mut data = 0;

        // Create value for mmDCC required for building JpmcdsBuildIRZeroCurve function call
        if JpmcdsStringToDayCountConv(
            CString::new("Act/360").expect("").into_raw(),
            &mut data as *mut _,
        ) != Success
        {
            panic!();
        }

        data
    };

    let dcc: c_long = unsafe {
        let mut dcc_data: i64 = 0;

        // Create value for dcc required for building JpmcdsBuildIRZeroCurve function call
        if JpmcdsStringToDayCountConv(
            CString::new("30/360").expect("").into_raw(),
            &mut dcc_data as *mut _,
        ) != Success
        {
            panic!("DayCountConv panicked");
        };
        dcc_data
    };

    let mut ivl: TDateInterval = unsafe {
        let mut ivl_data = TDateInterval {
            prd: 0 as raw::c_int,
            prd_typ: ' ' as raw::c_char,
            flag: 0 as raw::c_int,
        };

        // Create value for ivl required for building JpmcdsBuildIRZeroCurve function call
        if JpmcdsStringToDateInterval(
            CString::new("6M").expect("").into_raw(),
            routine.clone().into_raw(),
            &mut ivl_data as *mut _,
        ) != Success
        {
            panic!("Panic at creating interval!");
        };
        ivl_data
    };

    let freq: c_double = unsafe {
        let mut freq: f64 = 0.0;

        // Create value for freq required for building JpmcdsBuildIRZeroCurve function call
        if JpmcdsDateIntervalToFreq(&mut ivl as *mut TDateInterval, &mut freq as *mut _) != Success
        {
            panic!("Paniced at freq");
        };

        freq
    };

    let mut dates: Vec<TDate> = Vec::with_capacity(types.len());
    let base_date: TDate = unsafe { JpmcdsDate(2007, 3, 20) };
    let mut iter_ivl = TDateInterval {
        prd: 0,
        prd_typ: ' ' as raw::c_char,
        flag: 0,
    };

    let mut iter_date: TDate = unsafe { JpmcdsDate(2007, 3, 10) };
    let mut dates: Vec<TDate> = vec![];

    // Iter over types
    for i in 0..types.len() {
        let slot = CString::new(expires[i]).expect("");

        unsafe {
            if JpmcdsStringToDateInterval(
                slot.into_raw(),
                routine.clone().into_raw(),
                &mut iter_ivl as *mut _,
            ) != Success
            {
                panic!("panicked at slot");
            };

            if JpmcdsDateFwdThenAdjust(
                base_date,
                &mut iter_ivl as *mut _,
                'N' as c_long,
                CString::new("None").expect("None").into_raw(),
                &mut iter_date as *mut _,
            ) != Success
            {
                panic!("panicked at datefwd");
            };
        };
        dates.push(iter_date.clone());
    }

    let raw_zc = unsafe {
        let holidays = CString::new("None").expect("Could not create holidays!");
        let c_types = CString::new(types).expect("CString failed");

        let curve = JpmcdsBuildIRZeroCurve(
            base_date,
            c_types.into_raw(),
            dates.as_mut_ptr(),
            rates.as_mut_ptr(),
            types.len() as i64,
            mmDCC,
            freq as c_long,
            freq as c_long,
            dcc,
            dcc,
            'M' as i64,
            holidays.into_raw(),
        );
        curve
    };

    let zero_curve = unsafe { Box::from_raw(raw_zc) };

    zero_curve
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let rates = vec![
            0.004450, 0.009488, 0.012337, 0.017762, 0.019350, 0.020838, 0.016520, 0.020180,
            0.023033, 0.025250, 0.026960, 0.028250, 0.029310, 0.030170, 0.030920, 0.031600,
            0.032310, 0.033670, 0.034190, 0.034110, 0.034120,
        ];
        let expiries_vec = vec![
            "1M", "2M", "3M", "6M", "9M", "1Y", "2Y", "3Y", "4Y", "5Y", "6Y", "7Y", "8Y", "9Y",
            "10Y", "11Y", "12Y", "15Y", "20Y", "25Y", "30Y",
        ];

        let types = "MMMMMMSSSSSSSSSSSSSSS";

        let zero_curve = BuildZeroCurve(rates, expiries_vec, types);
        CalcUpfrontCharge(zero_curve, 500.0);
    }
}
