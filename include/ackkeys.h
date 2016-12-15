/**
 * This file contains the definitions for all keys with their respective
 * SDL_ScanCode and SDL_Key.
 *
 * It is used a an S-macro file to create a lot of automated variables and
 * functions.
 */
#ifdef _ACKKEY // _ACKKEY(name, keysym, scancode)

_ACKKEY(a, SDLK_a, SDL_SCANCODE_A)
_ACKKEY(b, SDLK_b, SDL_SCANCODE_B)
_ACKKEY(c, SDLK_c, SDL_SCANCODE_C)
_ACKKEY(d, SDLK_d, SDL_SCANCODE_D)
_ACKKEY(e, SDLK_e, SDL_SCANCODE_E)
_ACKKEY(f, SDLK_f, SDL_SCANCODE_F)
_ACKKEY(g, SDLK_g, SDL_SCANCODE_G)
_ACKKEY(h, SDLK_h, SDL_SCANCODE_H)
_ACKKEY(i, SDLK_i, SDL_SCANCODE_I)
_ACKKEY(j, SDLK_j, SDL_SCANCODE_J)
_ACKKEY(k, SDLK_k, SDL_SCANCODE_K)
_ACKKEY(l, SDLK_l, SDL_SCANCODE_L)
_ACKKEY(m, SDLK_m, SDL_SCANCODE_M)
_ACKKEY(n, SDLK_n, SDL_SCANCODE_N)
_ACKKEY(o, SDLK_o, SDL_SCANCODE_O)
_ACKKEY(p, SDLK_p, SDL_SCANCODE_P)
_ACKKEY(q, SDLK_q, SDL_SCANCODE_Q)
_ACKKEY(r, SDLK_r, SDL_SCANCODE_R)
_ACKKEY(s, SDLK_s, SDL_SCANCODE_S)
// TODO: Somehow shadow the type key_t with the variable key_t!
// _ACKKEY(t, SDLK_t, SDL_SCANCODE_T)
_ACKKEY(u, SDLK_u, SDL_SCANCODE_U)
_ACKKEY(v, SDLK_v, SDL_SCANCODE_V)
_ACKKEY(w, SDLK_w, SDL_SCANCODE_W)
_ACKKEY(x, SDLK_x, SDL_SCANCODE_X)
_ACKKEY(y, SDLK_y, SDL_SCANCODE_Y)
_ACKKEY(z, SDLK_z, SDL_SCANCODE_Z)

#else
#error ackkeys.h is meant to be used with the _ACKKEY macro defined!
#endif
