ModuleID = 'main'
source_filename = "main"

define i32 @sum(i32 %x, i32 %y) {
entry:
  %results = add i32 %x, %y
  ret i32 %results
}
