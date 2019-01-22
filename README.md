## Git submodule
```
git clone --recursive XXXXX.git 
```

OR do this after an original git clone

```
git submodule update --init --recursive
```

## RUN
```
mkdir build
cd build
ccmake ..
make -j8

ln -s ../7.3.camera.vs ./
ln -s ../7.3.camera.fs ./
```

## Current Process
* A HelloWorld ImGui on Haozhe's cut hole result

## Things need to be done
Try to see Repo->Boards (menu on hte left hand side of the repo). Since I did not test it yet, I'm not sure can you guys see those cards

* Split up Data & Drawing into different files
* Make Gui interactive (Input data from Gui rather than command line)
* Modify the Orbit control and Mouse Control module (refer to Nova)
* Fix the CMakeLists's imgui bug (see below)

## Bugs need to be fixed
1. ImGui CMake: Fix.
---

## Basic Git Branch Usage

[3.2 Git Branching - Basic Branching and Merging](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging)

* Create new branch
```
git checkout -b [new_branch_name]
```

and then you can do normal `add` and `commit`

* Push files to a specific branch
```
git push origin [branch_name]
```

**Most of the operations can be done via website**