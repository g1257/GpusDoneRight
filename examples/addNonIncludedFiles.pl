#!/usr/bin/perl -w

#modify this:
my $nvidiaSdk="/opt/NVIDIA_GPU_Computing_SDK";

###################################

my $rootDir="$nvidiaSdk/C/src/";

#pair of files, src, dest, src, dest...:
my @filePairs=(
"simpleTextureDrv/simpleTexture_kernel.cu,texture.cu",
"simpleTextureDrv/data/lena_bw.pgm,lena_bw.pgm",
"simpleTextureDrv/data/ref_rotated.pgm,ref_rotated.pgm"
);

chdir("../");
(-r "examples/") or die "$0: Error: You need to be in the directory examples/ under GpusDoneRight\n";

foreach my $filePair (@filePairs) {
	my @temp = split(/,/,$filePair);
	next if (-r "examples/$temp[1]"); # don't overwrite
	next unless (-r "$rootDir/$temp[0]"); # if source doesn't exist, continue
	system("cp -i $rootDir/$temp[0] examples/$temp[1]");
}

