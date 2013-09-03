#ifndef G_BREAKABLE_H_
#define G_BREAKABLE_H_

void breakable_use(gentity_t *self, gentity_t *other, gentity_t *activator);
void breakable_pain( gentity_t *self, gentity_t *attacker, int32_t damage );

#endif /* G_BREAKABLE_H_ */

