#ifndef G_BREAKABLE_H_
#define G_BREAKABLE_H_

void G_Breakable_InitBrush(gentity_t* ent);
void G_Breakable_Use(gentity_t* self, gentity_t* other, gentity_t* activator);
void G_Breakable_Pain(gentity_t* self, gentity_t* attacker, int32_t damage);

#endif /* G_BREAKABLE_H_ */

