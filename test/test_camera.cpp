#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#define GLM_FORCE_SWIZZLE
#include "common/camera.hpp"
//libs
#include <math.h>
#include <glm/gtx/string_cast.hpp>
#include <string>

namespace doctest {
  template<> struct StringMaker<glm::vec3> {
      static String convert(const glm::vec3& value) {
          return glm::to_string(value).c_str();
      }
  };

  template<> struct StringMaker<glm::vec2> {
      static String convert(const glm::vec2& value) {
          return glm::to_string(value).c_str();

      }
  };
}

glm::vec3 matrixTranform(const glm::mat4 &m, const glm::vec3& point){
      return (m * glm::vec4(point, 1.0f));
}


#define ARE_NAN(x, y) ( isnan(x) && isnan(y) )
#define EQUAL_OR_NAN(x, y) (ARE_NAN(x, y) || (x == doctest::Approx(y)))

bool mat4_are_equal(const glm::mat4 &matA, const glm::mat4 &matB)
{
return 
  ( EQUAL_OR_NAN( matA[0][0] , matB[0][0]) ) &&
  ( EQUAL_OR_NAN( matA[1][0] , matB[1][0]) ) &&
  ( EQUAL_OR_NAN( matA[2][0] , matB[2][0]) ) &&
  ( EQUAL_OR_NAN( matA[3][0] , matB[3][0]) ) &&

  ( EQUAL_OR_NAN( matA[0][1] , matB[0][1]) ) &&
  ( EQUAL_OR_NAN( matA[1][1] , matB[1][1]) ) &&
  ( EQUAL_OR_NAN( matA[2][1] , matB[2][1]) ) &&
  ( EQUAL_OR_NAN( matA[3][1] , matB[3][1]) ) &&

  ( EQUAL_OR_NAN( matA[0][2] , matB[0][2]) ) &&
  ( EQUAL_OR_NAN( matA[1][2] , matB[1][2]) ) &&
  ( EQUAL_OR_NAN( matA[2][2] , matB[2][2]) ) &&
  ( EQUAL_OR_NAN( matA[3][2] , matB[3][2]) ) &&

  ( EQUAL_OR_NAN( matA[0][3] , matB[0][3]) ) &&
  ( EQUAL_OR_NAN( matA[1][3] , matB[1][3]) ) &&
  ( EQUAL_OR_NAN( matA[2][3] , matB[2][3]) ) &&
  ( EQUAL_OR_NAN( matA[3][3] , matB[3][3]) );
}

const glm::vec3 cam_position = glm::vec3(5.0f, 5.0f, 5.0f);
const glm::vec3 cam_target = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

TEST_CASE("GetViewMatrix must return  camera matrix") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
 
  // act
  auto reference = glm::lookAt(cam_position, cam_target, cam_up);
  auto camview = cam.GetViewMatrix();

  // assert
  CHECK(mat4_are_equal(camview, reference) );  
}

TEST_CASE("GetPosition must return camera Position vector") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  
  // act
  
  // assert
  CHECK(cam.GetPosition() == cam_position);
}

TEST_CASE("GetTarget must return camera Target vector") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  
  // act
 
  // assert
  CHECK(cam.GetTarget() == cam_target);
}

TEST_CASE("getcameraDirection must return normalized vector pointing in the reverse direction of what it is targetingn") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  
  // act
  glm::vec3 frontvector = glm::normalize( cam_position - cam_target);
  glm::vec3 result = cam.getcameraDirection();
 
  // assert
  REQUIRE_MESSAGE(glm::length(result) == doctest::Approx(1.0f) , "Vector must be normalized");
  
  CHECK(result.x == doctest::Approx(frontvector.x));
  CHECK(result.y == doctest::Approx(frontvector.y));
  CHECK(result.z == doctest::Approx(frontvector.z));
}

TEST_CASE("getCameraRight must return normalized cross product camera_up and frontvector") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  
  // act
  glm::vec3 frontvector = glm::normalize(cam_position - cam_target);
  glm::vec3 cameraright = glm::normalize(glm::cross(cam_up, frontvector) );
  glm::vec3 result = cam.getcameraRight();
 
  // assert
  REQUIRE_MESSAGE(glm::length(result) == doctest::Approx(1.0f) , "Vector must be normalized");
  
  CHECK(result.x == doctest::Approx(cameraright.x));
  CHECK(result.y == doctest::Approx(cameraright.y));
  CHECK(result.z == doctest::Approx(cameraright.z));
}


TEST_SUITE("Camera Fov()") {
  TEST_CASE("cameraFov(+/-offset) must change Fov") {
    // arrange
    Camera cam(cam_position, cam_target, cam_up);
    float offset = 1.0f;
    
    SUBCASE("Fov must increment"){
        // act
        float fov = cam.GetFov() + offset;
        cam.cameraFov(offset);
        // assert
        CHECK(cam.GetFov() == fov);
    }
    SUBCASE("Fov must decrement"){
        // act
        float fov = cam.GetFov() - offset;
        cam.cameraFov(-offset);
        // assert
        CHECK(cam.GetFov() == fov);
    }
  }

  TEST_CASE("cameraFov() range must be from 2 to 178  ") {
    // arrange
    float maxfov = 178;
    float minfov = 2;
    float increment = 2;
    Camera cam_maxfov(cam_position, cam_target, cam_up, maxfov);
    Camera cam_minfov(cam_position, cam_target, cam_up, minfov);
    
    SUBCASE("Fov must be >= 2"){
        // act max value
        cam_maxfov.cameraFov(increment);
        // assert mix
        CHECK(cam_maxfov.GetFov() <= maxfov);
    }

    SUBCASE("Fov must be <= 180"){
        // act min value
        cam_maxfov.cameraFov(-increment);
        // assert max
        CHECK(cam_minfov.GetFov() >= minfov);
    }
  }
}// end Fov

TEST_CASE("cameraPan() must move cam position and target") {
  // arrange
  auto offset = glm::vec2(1.0f, 1.0f);
  Camera cam(cam_position, cam_target, cam_up);
  glm::vec3 cameraDirection = glm::normalize(cam_position - cam_target);
  glm::vec3 cameraRight = glm::normalize(glm::cross(cam_up, cameraDirection));
  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
  glm::mat4 tx , ty;


  // act  
  cam.cameraPan(offset.x, offset.y );

  tx = glm::translate(glm::mat4(1.0), cameraRight * -offset.x);
  ty = glm::translate(glm::mat4(1.0), cam_up * offset.y );
  auto t = tx * ty;
  glm::vec3 new_pos    = matrixTranform( t, cam_position );
  glm::vec3 new_target = matrixTranform( t, cam_target );

  // assert
  CHECK(cam.GetPosition()  == new_pos);
  CHECK(cam.GetTarget()    == new_target);

}

TEST_CASE("cameraPan() move x == -x , move y == y") {
  // arrange
  std::vector<Camera> cams{};
  cams.push_back(Camera( glm::vec3(   0.0f,  0.0f,  1.0f), cam_target, cam_up) ); // xy front
  cams.push_back(Camera( glm::vec3(   1.0f,  0.0f,  0.0f), cam_target, cam_up) ); // zy left
  cams.push_back(Camera( glm::vec3(  -1.0f,  0.0f,  0.0f), cam_target, cam_up) ); // zy right
  cams.push_back(Camera( glm::vec3(   0.0f,  0.0f, -1.0f), cam_target, cam_up) ); // xy back

  glm::vec2 offset = glm::vec2(1.0f, 1.0f);
  for(auto cam : cams){
    // arrange
    auto startpoint = matrixTranform(cam.GetViewMatrix(), glm::vec3(0.0f)); 

    // act  
    cam.cameraPan( offset.x, offset.y );
    auto endpoint = matrixTranform(cam.GetViewMatrix(), glm::vec3(0.0f));
    glm::vec2 result = ( startpoint - endpoint ).xy;

    // assert
    CHECK( offset.x == doctest::Approx(-result.x) );
    CHECK( offset.y == doctest::Approx(result.y) );
  }
}

TEST_CASE("cameraOrbit() offset xy must rotate around cameraright then around cam_up") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  glm::vec2 offset = glm::vec2(0.1f, 0.1f);
  auto view_mat = glm::lookAt(cam_position, cam_target, cam_up);
  
  // act
  auto reference = glm::rotate(view_mat, offset.y, cam.getcameraRight());
  reference = glm::rotate(reference, offset.x, cam_up);
  cam.cameraOrbit(offset.x, offset.y);
  auto result = cam.GetViewMatrix();

  // assert
  CHECK(mat4_are_equal(result , reference) );
}

TEST_CASE("cameraDolly() offset must move camera toward -cameradirection") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  glm::vec2 offset = glm::vec2(0.1f, 0.1f);
  float step = (abs(offset.x) > abs(offset.y)) ? offset.x : offset.y;
  auto view_mat = glm::lookAt(cam_position, cam_target, cam_up);
  
  // act
  auto reference = glm::translate(view_mat, -cam.getcameraDirection() * step);
  cam.cameraDolly(offset.x, offset.y);
  auto result = cam.GetViewMatrix();

  // assert
  CHECK(mat4_are_equal(result , reference) );
}

TEST_CASE("cameraRoll() offset must rotate camera around -cameradirection") {
  // arrange
  Camera cam(cam_position, cam_target, cam_up);
  glm::vec2 offset = glm::vec2(0.1f, 0.1f);
  float step = (abs(offset.x) > abs(offset.y)) ? offset.x : offset.y;
  auto view_mat = glm::lookAt(cam_position, cam_target, cam_up);
  
  // act
  auto reference = glm::rotate(view_mat, step, -cam.getcameraDirection());
  cam.cameraRoll(offset.x, offset.y);
  auto result = cam.GetViewMatrix();

  // assert
  CHECK(mat4_are_equal(result , reference) );
}

TEST_CASE("") {
  // arrange

  // act  

  // assert

}









