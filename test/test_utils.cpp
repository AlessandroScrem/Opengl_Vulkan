#include "doctest.h"
// common lib
#include <utils.hpp>

//libs
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

TEST_CASE("") {
  // arrange

  // act  

  // assert
}

void InitializeMouseState(){
   ngn::Mouse::Move(0,0);
   ngn::Mouse::Stop();
}

TEST_CASE("When mouse::Start() ,  getDirection == zero ") {
  // arrange
  // act
  InitializeMouseState();
  ngn::Mouse::Start();

  REQUIRE(ngn::Mouse::getDirection_str() == "Free");
  REQUIRE(ngn::Mouse::isClicked() == true);
  
  // assert
  CHECK(ngn::Mouse::getDirection().x ==  0.0f);
  CHECK(ngn::Mouse::getDirection().y ==  0.0f);
}

TEST_CASE("When Mouse::Stop , direction is Free ") {
  // arrange
  InitializeMouseState();
  ngn::Mouse::Start();
  REQUIRE(ngn::Mouse::isClicked() == true);
  // act

  ngn::Mouse::Stop();
  REQUIRE(ngn::Mouse::isClicked() == false);

  // assert
  CHECK(ngn::Mouse::getDirection_str() == "Free");
}

TEST_CASE("Mouse movements") {
  // arrange
  InitializeMouseState();
  glm::vec2 move{};
  glm::vec2 result{};
  ngn::Mouse::Start();
  ngn::Mouse::Move(100, 120);
  ngn::Mouse::Move(0, 0);

  SUBCASE("positive movement move must return offset"){
    // arrange
    move = {10,10};
    result = move * ngn::Mouse::getMouseSensitivity();

    // act
    ngn::Mouse::Move(move.x, move.y);

    // assert
    CHECK(ngn::Mouse::getDirection() == result);
    SUBCASE("negative movement must return negative offset "){
      // arrange
      auto offset = glm::vec2{-5, -5};
      move += offset;
      result = offset * ngn::Mouse::getMouseSensitivity();

      // act
      ngn::Mouse::Move(move.x, move.y);

      // assert
      CHECK(ngn::Mouse::getDirection() == result);
      SUBCASE("zero movement  must return zero"){
        // arrange
        move += glm::vec2{0, 0};

        // act
        ngn::Mouse::Move(move.x, move.y);

        // assert
        CHECK(ngn::Mouse::getDirection().x == 0);
        CHECK(ngn::Mouse::getDirection().y == 0);
      }
    }
  }

}

TEST_CASE("Mouse::Move() straight ") {
  // arrange
  InitializeMouseState();
  ngn::Mouse::Start();
  float scale = ngn::Mouse::getMouseSensitivity();
  const float delta = 31;
  glm::vec2 move{};
  glm::vec2 result{};
  std::string direction("Free");

  SUBCASE("when delta > 30 "){
    SUBCASE("x>delta && y<3 -> straightX "){
      // arrange 
      move = {delta, 0};
      result = { (delta * scale), 0.0f};
      direction = "straightX";
    }
    SUBCASE("negative X movements"){
      // arrange
      move = {-delta, 0};
      result = {(-delta * scale), 0.0f};
      direction = "straightX";
    }
    SUBCASE("y>delta -> straightY"){
      // arrange
      move = {0, delta};
      result = {0.0f, (delta * scale)};
      direction = "straightY";
    }
    SUBCASE("negative Y movements"){
      // arrange
      move = {0, -delta};
      result = {0.0f, (-delta * scale)};
      direction = "straightY";
    }
    
    // act
    ngn::Mouse::Move(move.x, move.y);
    REQUIRE(ngn::Mouse::getDirection_str() == direction);

    // assert
    CHECK(ngn::Mouse::getDirection().x == result.x);
    CHECK(ngn::Mouse::getDirection().y == result.y);
    // reset mouse conditions
    ngn::Mouse::Stop();
  }

}
