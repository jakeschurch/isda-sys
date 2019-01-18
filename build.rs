extern crate bindgen;
extern crate cc;

use std::env;
use std::path::PathBuf;

pub fn main() {
    cc::Build::new()
        .file("isda_src/lib/src/yearfrac.c")
        .file("isda_src/lib/src/zcall.c")
        .file("isda_src/lib/src/strutil.c")
        .file("isda_src/lib/src/version.c")
        .file("isda_src/lib/src/zcswdate.c")
        .file("isda_src/lib/src/streamcf.c")
        .file("isda_src/lib/src/zcswap.c")
        .file("isda_src/lib/src/zcswdate.h")
        .file("isda_src/lib/src/zr2fwd.c")
        .file("isda_src/lib/src/tcurve.c")
        .file("isda_src/lib/src/stub.c")
        .file("isda_src/lib/src/timeline.c")
        .file("isda_src/lib/src/zerocurve.c")
        .file("isda_src/lib/src/zr2coup.c")
        .file("isda_src/lib/src/zcswutil.c")
        .file("isda_src/lib/src/ldate.c")
        .file("isda_src/lib/src/rtbrent.c")
        .file("isda_src/lib/src/cashflow.c")
        .file("isda_src/lib/src/cx.c")
        .file("isda_src/lib/src/bsearch.c")
        .file("isda_src/lib/src/bsearch.inc")
        .file("isda_src/lib/src/fltrate.c")
        .file("isda_src/lib/src/busday.c")
        .file("isda_src/lib/src/cxdatelist.c")
        .file("isda_src/lib/src/cxbsearch.c")
        .file("isda_src/lib/src/cxzerocurve.c")
        .file("isda_src/lib/src/lscanf.c")
        .file("isda_src/lib/src/lintrp1.inc")
        .file("isda_src/lib/src/defaulted.c")
        .file("isda_src/lib/src/schedule.c")
        .file("isda_src/lib/src/datelist.c")
        .file("isda_src/lib/src/cmemory.c")
        .file("isda_src/lib/src/cds.c")
        .file("isda_src/lib/src/lprintf.c")
        .file("isda_src/lib/src/lintrp1.c")
        .file("isda_src/lib/src/cdsone.c")
        .file("isda_src/lib/src/gtozc.c")
        .file("isda_src/lib/src/feeleg.c")
        .file("isda_src/lib/src/cfileio.c")
        .file("isda_src/lib/src/badday.c")
        .file("isda_src/lib/src/interpc.c")
        .file("isda_src/lib/src/dateadj.c")
        .file("isda_src/lib/src/dtlist.c")
        .file("isda_src/lib/src/buscache.c")
        .file("isda_src/lib/src/cerror.c")
        .file("isda_src/lib/src/convert.c")
        .file("isda_src/lib/src/date_sup.c")
        .file("isda_src/lib/src/cdsbootstrap.c")
        .file("isda_src/lib/src/contingentleg.c")
        .file("isda_src/lib/src/cfinanci.cpp")
        .file("isda_src/lib/src/linterpc.c")
        .file("isda_src/lib/src/dateconv.c")
        .include("isda_src/lib/include/isda")
        .static_flag(true)
        .compile("libcds.a");

    let bindings = bindgen::Builder::default()
        .header("isda_src/lib/include/isda/wrapper.h")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!")
}
