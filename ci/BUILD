package(default_visibility = ["//visibility:public"])

load("@com_intel_plaidml//bzl:conda.bzl", "conda_binary")

conda_binary(
    name = "report",
    srcs = [
        "report.py",
        "util.py",
    ],
    data = glob([
        "golden/**/*",
        "plan.yml",
        "templates/*",
    ]),
    env = "@com_intel_plaidml_conda_analysis//:env",
    tags = ["manual"],
    deps = ["//tools/analysis"],
)
