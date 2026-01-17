# Acoustic Fluids

![C++](https://img.shields.io/badge/C++-23-5599ff?logo=c%2B%2B) ![SDL3](https://img.shields.io/badge/SDL-3.0-orange) [![Windows Build](https://github.com/Krish2882005/Acoustic-Fluids/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/Krish2882005/Acoustic-Fluids/actions/workflows/cmake-single-platform.yml) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=Krish2882005_Acoustic-Fluids&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=Krish2882005_Acoustic-Fluids) ![Status](https://img.shields.io/badge/Status-Development-green)

> **Real-time Navier-Stokes fluid simulation driven by audio frequency analysis.**

**Acoustic Fluids** is a real-time simulation built with C++23 and SDL3. It implements an Eulerian fluid solver executed on the GPU via Compute Shaders, where physical parameters (force, density, vorticity) are modulated by low-latency audio FFT data.
