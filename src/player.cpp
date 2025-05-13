#include "player.h"

Player::Player(const int x, const int y)
: _tile_pos_x(x), _tile_pos_y(y) {}

Player::~Player() {
    SDL_DestroyTexture(_texture);
}
void Player::set_texture(SDL_Texture * texture) {
    _texture = texture;
}

void Player::move(const Direction d) {
    unhide();
    switch (d) {
        case UP:
            --_tile_pos_y;
            break;
        case DOWN:
            ++_tile_pos_y;
            break;
        case LEFT:
            --_tile_pos_x;
            break;
        case RIGHT:
            ++_tile_pos_x;
            break;
    }
}

void Player::blink(const uint64_t delta) {
    _blink_timer += delta;
    if (_blink_timer > 1200 & !_hidden | _blink_timer > 300 & _hidden) {
        toggle_hidden();
    }
}

void Player::toggle_hidden() {
    _hidden = !_hidden;
    _blink_timer = 0;
}

void Player::hide() {
    _hidden = true;
    _blink_timer = 0;
}

void Player::unhide() {
    _hidden = false;
    _blink_timer = 0;
}

bool Player::is_hidden() const {
    return _hidden;
}

SDL_Texture * Player::get_texture() const {
    return _texture;
}

Vector2D_Int Player::get_destination_coords() const {
    return { _tile_pos_x, _tile_pos_y};
}

Vector2D_Int Player::get_texture_coords() const {
    return {0,0};
}
