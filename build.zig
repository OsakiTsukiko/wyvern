const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{ .default_target = .{
        .abi = .none,
        .cpu_arch = .aarch64,
        .os_tag = .freestanding,
    } });
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/kernal.zig"),
        .target = target,
        .optimize = optimize,
    });
    // exe_mod.addIncludePath(b.path("src/"));
    exe_mod.addAssemblyFile(b.path("src/boot.S"));
    // exe_mod.addCSourceFile(.{ .file = b.path("src/kernel.c") });
    exe_mod.addCSourceFiles(.{ .root = b.path("src"), .files = &[_][]const u8{
        "core/io.c",
        "core/mb.c",
        "core/fb.c",
    } });

    const exe = b.addExecutable(.{
        .name = "kernel8.img",
        .root_module = exe_mod,
    });
    exe.addIncludePath(b.path("src/"));
    exe.setLinkerScript(b.path("src/link.ld"));

    b.installArtifact(exe);

    const run_cmd = b.addSystemCommand(&[_][]const u8{
        "qemu-system-aarch64",
        "-M",
        "raspi4b",
        "-m",
        "2G",
        "-serial",
        "stdio",
        "-kernel",
    });
    run_cmd.addFileArg(exe.getEmittedBin());
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("qemu", "Run the kernel in qemu.");
    run_step.dependOn(&run_cmd.step);
}
