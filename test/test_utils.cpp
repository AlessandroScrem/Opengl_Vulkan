#include "doctest.h"
#include "common/Input/utils.hpp"

TEST_CASE("") {
  // arrange

  // act  

  // assert
}

TEST_CASE("When mouse::Start() is called -> Mouse::isClicked() is true") {
  // arrange
  // act
  ngn::Mouse::Start();

  REQUIRE(ngn::Mouse::getDirection_str() == "Free");
  
  // assert
  CHECK(ngn::Mouse::isClicked() == true);
}




TEST_CASE("When Mouse::Start && Mouse::Stop direction is Free ") {
  // arrange
  ngn::Mouse::Move(0,0);
  ngn::Mouse::Start();
  ngn::Mouse::getDirection();
  REQUIRE(ngn::Mouse::isClicked() == true);
  REQUIRE(ngn::Mouse::getDirection_str() == "Free");
  // act

  ngn::Mouse::Stop();

  // assert
  CHECK(ngn::Mouse::getDirection_str() == "Free");
  CHECK(ngn::Mouse::isClicked() == false);
}

TEST_CASE("When Mouse::Move()") {
  // arrange
  float scale = ngn::Mouse::getMouseSensitivity();
  glm::vec2 move{};
  glm::vec2 result{};
  std::string direction("Free");
  ngn::Mouse::Move(0,0);
  ngn::Mouse::Start();
  ngn::Mouse::getDirection();
  REQUIRE(ngn::Mouse::isClicked() == true);
  REQUIRE(ngn::Mouse::getDirection_str() == "Free");

  SUBCASE(""){
    SUBCASE("x>30 && y<3 -> straightX "){
      // arrange 
      move = {31, 0};
      result = { (move.x * scale), 0.0f};
      direction = "straightX";
    }
    SUBCASE("negative X movements"){
      // arrange
      move = {-31, 0};
      result = {(move.x * scale), 0.0f};
      direction = "straightX";
    }
    SUBCASE("x<3 && y>30 -> straightY"){
      // arrange
      move = {0, 31};
      result = {0.0f, (move.y * scale)};
      direction = "straightY";
    }
    SUBCASE("negative Y movements"){
      // arrange
      move = {0, -31};
      result = {0.0f, (move.y * scale)};
      direction = "straightY";
    }
    
    ngn::Mouse::Move(move.x, move.y);
    auto offset = ngn::Mouse::getDirection();
    REQUIRE(ngn::Mouse::getDirection_str() == direction);
    REQUIRE(ngn::Mouse::isClicked() == true);
    ngn::Mouse::Stop();

    // assert
    CHECK(offset.x == result.x);
    CHECK(offset.y == result.y);
  }

  CHECK(ngn::Mouse::getDirection_str() == "Free");
  CHECK(ngn::Mouse::isClicked() == false);
}
