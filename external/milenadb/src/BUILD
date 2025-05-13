cc_library(
    name = "milenadb_queue",
    srcs = ["milena_queue.cpp"],
    hdrs = ["milena_queue.h"],
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "milena_queue_test",
    srcs = ["milena_queue_test.cpp"],
    deps = [":milenadb_queue"],
) 