const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{ .default_target = .{
        .abi = .none,
        .cpu_arch = .aarch64,
        .os_tag = .freestanding,
    } });
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });
    exe_mod.addAssemblyFile(b.path("src/boot.S"));
    exe_mod.addCSourceFile(.{ .file = b.path("src/kernel.c") });

    const exe = b.addExecutable(.{
        .name = "wyvern",
        .root_module = exe_mod,
    });
    exe.setLinkerScript(b.path("src/link.ld"));

    b.installArtifact(exe);
}
