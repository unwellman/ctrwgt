#ifndef ACTOR_H
#define ACTOR_H

struct actor;
struct actor_message;
struct actor_component;

// Actor self-updates based on a time step
void actor_update (struct actor *a, float dt);

// Send a message from one actor to another
void actor_message (struct actor *src, struct actor *dst, struct actor_message
	data);

#endif

