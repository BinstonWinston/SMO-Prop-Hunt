#!/bin/bash
BASE_RYU_PATH="/mnt/c/Users/Binston/AppData/Roaming/Ryujinx/sdcard"
make -j && \
cp starlight_patch_100/atmosphere/exefs_patches/StarlightBase/3CA12DFAAF9C82DA064D1698DF79CDA1.ips "$BASE_RYU_PATH/atmosphere/exefs_patches/StarlightBase"
cp starlight_patch_100/atmosphere/contents/0100000000010000/exefs/subsdk1 "$BASE_RYU_PATH/atmosphere/contents/0100000000010000/exefs"
cp -r starlight_patch_100/atmosphere/contents/0100000000010000/romfs "$BASE_RYU_PATH/atmosphere/contents/0100000000010000"
