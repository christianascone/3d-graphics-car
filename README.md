3d-graphics-car
===============

[![GitHub release](https://img.shields.io/github/release/christianascone/3d-graphics-car.svg?maxAge=2592000)](https://github.com/christianascone/3d-graphics-car/releases/latest)
[![Apache2.0 licensed](https://img.shields.io/github/license/christianascone/3d-graphics-car.svg)](https://github.com/christianascone/3d-graphics-car/blob/master/LICENSE)

3D computer graphics project which simulates a simple game with a car

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

Install this packages (linux environment):

 - g++
 - libsdl2-dev
 - libsdl2-ttf-dev
 - libsdl2-image-dev
 - libsdl2-dev
 - libsdl2-ttf-dev
 - libsdl2-image-dev
 - freeglut3-dev
 - libgl1-mesa-glx
 - mesa-utils

Let's compile project.
Move inside `progettoCar` directory and run `make`:
```shell
cd shared_data/src/progettoCar
make
```

### Running

After compiling project with success, it is ready for run:
```shell
./progettoCar
```

## Deployment

The project includes a basic VagrantFile with needed dependencies and `virtualbox` set as provider, but [VmWare](http://vmware.com) is a better choice for better performance.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/christianascone/3d-graphics-car/tags). 


## License

This project is licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) - see the [LICENSE](LICENSE) file for details

## Acknowledgments

* Neither the 3D model nor textures are owned by me
* This project was born for [Computer Graphics Course](http://www.unibo.it/en/teaching/course-unit-catalogue/course-unit/2015/324634) in [University of Bologna](http://www.unibo.it/en/homepage) (Academic Year 2015/2016). 
	`GCGraLib2` and `progetto_car_1516` code has been provided by [Course's teacher](https://www.unibo.it/sitoweb/giulio.casciola/en)