; ModuleID = 'foo.c'
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.10.0"

%struct.foo_struct = type { i32, double, double }

; Function Attrs: nounwind ssp uwtable
define void @myfunc(%struct.foo_struct* noalias sret %agg.result, i32 %a, i32 %b) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %f = alloca %struct.foo_struct, align 8
  store i32 %a, i32* %1, align 4
  store i32 %b, i32* %2, align 4
  %3 = getelementptr inbounds %struct.foo_struct* %f, i32 0, i32 0
  %4 = load i32* %1, align 4
  store i32 %4, i32* %3, align 4
  %5 = getelementptr inbounds %struct.foo_struct* %f, i32 0, i32 1
  store double 5.000000e+00, double* %5, align 8
  %6 = getelementptr inbounds %struct.foo_struct* %f, i32 0, i32 2
  store double 6.000000e+00, double* %6, align 8
  %7 = bitcast %struct.foo_struct* %agg.result to i8*
  %8 = bitcast %struct.foo_struct* %f to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* %8, i64 24, i32 8, i1 false)
  ret void
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

; Function Attrs: nounwind ssp uwtable
define i32 @main() #0 {
  %d = alloca double, align 8
  %1 = alloca %struct.foo_struct, align 8
  call void @myfunc(%struct.foo_struct* sret %1, i32 2, i32 53)
  %2 = getelementptr inbounds %struct.foo_struct* %1, i32 0, i32 2
  %3 = load double* %2, align 8
  store double %3, double* %d, align 8
  ret i32 0
}

attributes #0 = { nounwind ssp uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Apple LLVM version 6.1.0 (clang-602.0.53) (based on LLVM 3.6.0svn)"}
