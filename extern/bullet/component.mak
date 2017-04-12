TARGETS := EXTERN.BULLET

EXTERN.BULLET.TYPE                     := static-lib
EXTERN.BULLET.NAME                     := funner.extern.bullet
EXTERN.BULLET.SOURCE_DIRS              := sources/LinearMath sources/BulletCollision/BroadphaseCollision sources/BulletCollision/CollisionDispatch \
                                          sources/BulletCollision/CollisionShapes sources/BulletCollision/Gimpact \
                                          sources/BulletCollision/NarrowPhaseCollision sources/BulletDynamics/Character \
                                          sources/BulletDynamics/ConstraintSolver sources/BulletDynamics/Dynamics sources/BulletDynamics/Vehicle
EXTERN.BULLET.IMPORTS                  := compile.extern.bullet
EXTERN.BULLET.g++.COMPILER_CFLAGS      := --no-warn
EXTERN.BULLET.clang.COMPILER_CFLAGS    := -w
EXTERN.BULLET.msvc.COMPILER_CFLAGS     := -wd4305 -wd4244
EXTERN.BULLET.vcx86-64.COMPILER_CFLAGS := -wd4267
EXTERN.BULLET.bada.IMPORTS             := compile.extern.bada
EXTERN.BULLET.emscripten.COMPILER_CFLAGS := -Wno-overloaded-virtual -Wno-reorder -Wno-unused-private-field -Wno-unused-variable