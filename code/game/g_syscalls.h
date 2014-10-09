#ifndef _G_SYSCALLS_H
#define _G_SYSCALLS_H

#include "g_local.h"

/**
* Call G_PRINT in the engine.
* \param fmt format string.
*/
void trap_Printf(const char* fmt);

/**
*	Calls Com_error in the engine
*	\param fmt error desription
*/
void trap_Error(const char* fmt);

/**
*	Get milliseconds since engine start
*	\return milliseconds since engine start
*/
int trap_Milliseconds(void);

/**
*	Get count of arguments for the current client game command
*	\return count of arguments
*/
int trap_Argc(void);

/**
*	Get a n of the current client game command
*	\param n argument to get
*	\param buffer buffer to store the argument in
*	\param bufferLength size of the buffer
*/
void trap_Argv(int n, char* buffer, int bufferLength);

/**
*	Get all args of the current client game command
*	\param buffer buffer to store the arguments in
*	\param bufferLength size of the buffer
*/
void trap_Args(char* buffer, int bufferLength);

/**
*	Opens a file
*	\param qpath path and filename
*	\param f filehandle to use
*	\param mode mode to use
*/
int trap_FS_FOpenFile(const char* qpath, fileHandle_t* f, fsMode_t mode);

/**
*	Read a opened file
*	\param buffer buffer to read to
*	\param len buffer length or length to read (<sizeof(buffer))
*	\param f filehandle for the file to read from
*
*	You have to open the file first.
*/
void trap_FS_Read(void* buffer, int len, fileHandle_t f);

/**
*	Write to a file
*	\param buffer text to write
*	\param len length of buffer
*	\param f filehandle  for the file to write to
*
*	You have to open the file first.
*/
void trap_FS_Write(const void* buffer, int len, fileHandle_t f);

/**
*	Close a file
*	\param f filehandle for file to close
*/
void trap_FS_FCloseFile(fileHandle_t f);

/**
*	Get a list of files in a path
*	\param path path to get the list for
*	\param extension filter to get only files with this extension
*	\param listbuf buffer to store the file list in
*	\param bufsize size of the buffer
*	\return number of files in the list
*/
int trap_FS_GetFileList(const char* path, const char* extension, char* listbuf, int bufsize);

/**
*	Sends a console command to execute to the client console
*	\param exec_when when to exec (e.g. EXEC_APPEND)
*	\param text the command to execute
*/
void trap_SendConsoleCommand(int exec_when, const char *text);

/**
*	Register a cvar
*	\param cvar representation of the cvar in the vm
*	\param var_name name of the cvar
*	\param value default value for the cvar
*	\param flags additional options for the cvar (e.g. CVAR_ARCHIVE)
*/
void trap_Cvar_Register( /*@null@*/ vmCvar_t* cvar, const char* var_name, const char* value, int flags);

/**
*	\brief Update a cvar.
*
*	Tells the server/engine that a cvar in the vm has changed.
*	\param cvar cvar to update
*/
void trap_Cvar_Update(vmCvar_t* cvar);

/**
*	Set the cvar to a value.
*	\param var_name name of the cvar to set
*	\param value new value for the cvar
*/
void trap_Cvar_Set(const char* var_name, const char* value);

/**
*	Get the integer value for an cvar
*	\param var_name name of the cvar
*/
int trap_Cvar_VariableIntegerValue(const char* var_name);

/**
*	Get the value of the cvar as string
*	\param var_name name of the cvar
*	\param buffer to store the value
*	\param bufsize size of the buffer
*/
void trap_Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize);

/**
*	Send some information of the current game/map to the server
*	\param pointer to level.entities which is g_entities
*	\param numGEntities number of game entities (level.num_entities)
*	\param sizeofGEntity_t size of gentity_t
*	\param gameClients level.clients[0].ps
*	\param sizeOfGameClient size of level.clients[0]
*/
void trap_LocateGameData(gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* gameClients, int sizeofGameClient);

/**
*	Drop a client from server.
*	\param clientNum client number of client to drop
*	\param test reason for client drop
*/
void trap_DropClient(int clientNum, const char* reason);

/**
*	\brief Send a server command to the client
*	\param clientNum client number of client
*	\param server command to execute
*
*	A value of -1 for clientNum will send the command to all clients.
*/
void trap_SendServerCommand(int clientNum, const char* text);

/**
*	Set a configstring
*	\param num CS_...
*	\param string set cofig string to this
*/
void trap_SetConfigstring(int num, /*@null@*/ const char* string);

/**
*	Get a configstring
*	\param num CS_...
*	\param buffer buffer to store config string in
*	\param bufferSize size of buffer
*/
void trap_GetConfigstring(int num, char* buffer, size_t bufferSize);

/**
*	Get the userinfo for a client
*	\param num client number
*	\param buffer buffer to store config string in
*	\param size of buffer
*/
void trap_GetUserinfo(int num, char* buffer, int bufferSize);

/**
*	Set the userinfo for a client
*	\param num client number
*	\param buffer string the contains new userinfo
*/
void trap_SetUserinfo(int num, const char* buffer);

/**
*	Get server info.
*	\param buffer buffer to store the info in
*	\param bufferSize size of buffer
*/
void trap_GetServerinfo(char* buffer, size_t bufferSize);

/**
*	\brief Set the brush model for a entity.
*	\param ent entity to the the model on
*	\param name the model name
*
*	The normal case is trap_SetBrushModel(ent, ent->model).
*	Brush models always have names of *<int>.
*/
void trap_SetBrushModel(gentity_t* ent, const char* name);

/**
*	\brief Do a trace on the server
*	\param results trace_t to store the results in
*	\param start startpoint
*	\param end endpoint
*	\param mins used to define the volume size
*	\param maxs uses to define the volume size
*	\param passEntityNum entity/entities to ignore
*	\param contentmask only check for this contents (CONTENT_...)
*
*	This actually moves a box with the size defined by mins and maxs through the world
*	from start to end and checks whether it colides with anything that matches the contentmask.
*	The entities that math the passEntityNum will be ingnored.
*/
void trap_Trace(trace_t* results, const vec3_t start, /*@null@*/ const vec3_t mins, /*@null@*/ const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask);

/**
*	\param point the point
*	\param passEntityNum ingore this
*
*	Works similar to a trace but only check a single point.
*/
int trap_PointContents(const vec3_t point, int passEntityNum);

qboolean trap_InPVS(const vec3_t p1, const vec3_t p2);

qboolean trap_InPVSIgnorePortals(const vec3_t p1, const vec3_t p2);

/**
*	Adjust the state of a area portal used for doors etc
*	\param ent entity that effects the areaportal (area portal is inide the entities bounds)
*	\param open open or close it?
*/
void trap_AdjustAreaPortalState(gentity_t* ent, qboolean open);

/**
*	Checks if two areas are connected.
*/
qboolean trap_AreasConnected(int area1, int area2);

/**
*	Link an entity.
*	This results in shared values beeing avaible on both game and client side.
*/
void trap_LinkEntity(gentity_t* ent);

/**
*	Unlinks an entity.
*/
void trap_UnlinkEntity(gentity_t* ent);

/**
*	\brief Get a list of all entities in a box.
*	\param entityList list where entitynums will be stored
*	\param maxcount limits the number of listed entities
*
*	The size of the box is defined by mins and maxs.
*/
int trap_EntitiesInBox(const vec3_t mins, const vec3_t maxs, int32_t* entityList, int maxcount);

/**
*	Checks if a entity is in contact with a defined box.
*/
qboolean trap_EntityContact(const vec3_t mins, const vec3_t maxs, const gentity_t* ent);

/**
*	Allocates a free client for a bot.
*/
int trap_BotAllocateClient(void);

/**
*	Free the client that was used for a bot.
*/
void trap_BotFreeClient(int clientNum);

/**
*	Get the last command a user did.
*/
void trap_GetUsercmd(int clientNum, usercmd_t* cmd);

/**
* Get entity token.
*/
qboolean trap_GetEntityToken(char* buffer, int bufferSize);

/**
* Create a debug polygon.
*
* \param color color of the polygon
* \param numPoints number of points the polygon has
* \param points points of the polygon
* \return Polgon id
*/
int trap_DebugPolygonCreate(int color, int numPoints, vec3_t* points);

/**
* Deletes a debug polygon.
*
* \param id id of polygon to delete
*/
void trap_DebugPolygonDelete(int id);

int	trap_BotLibSetup(void);

int	trap_BotLibShutdown(void);

int	trap_BotLibVarSet(char* var_name, char* value);

int	trap_BotLibVarGet(char* var_name, char* value, int size);

int	trap_BotLibDefine(char* string);

int	trap_BotLibStartFrame(float time);

int	trap_BotLibLoadMap(const char* mapname);

int	trap_BotLibUpdateEntity(int ent, void* bue);

int	trap_BotLibTest(int parm0, char* parm1, vec3_t parm2, vec3_t parm3);

int	trap_BotGetSnapshotEntity(int clientNum, int sequence);

int	trap_BotGetConsoleMessage(int clientNum, char* message, int size);

void trap_BotUserCommand(int client, usercmd_t* ucmd);

void trap_AAS_EntityInfo(int entnum, void* info);

int	trap_AAS_Initialized(void);

void trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);

float trap_AAS_Time(void);

int	trap_AAS_PointAreaNum(vec3_t point);

int	trap_AAS_TraceAreas(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas);

int	trap_AAS_PointContents(vec3_t point);

int	trap_AAS_NextBSPEntity(int ent);

int	trap_AAS_ValueForBSPEpairKey(int ent, char* key, char* value, int size);

int	trap_AAS_VectorForBSPEpairKey(int ent, char* key, vec3_t v);

int	trap_AAS_FloatForBSPEpairKey(int ent, char* key, float* value);

int	trap_AAS_IntForBSPEpairKey(int ent, char* key, int* value);

int	trap_AAS_AreaReachability(int areanum);

int	trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);

int	trap_AAS_Swimming(vec3_t origin);

int	trap_AAS_PredictClientMovement(void* move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);

void trap_EA_Say(int client, char* str);

void trap_EA_SayTeam(int client, char* str);

void trap_EA_SayClass(int client, char* str);

void trap_EA_UseItem(int client, char* it);

void trap_EA_DropItem(int client, char* it);

void trap_EA_UseInv(int client, char* inv);

void trap_EA_DropInv(int client, char* inv);

void trap_EA_Gesture(int client);

void trap_EA_Command(int client, char* command);

void trap_EA_SelectWeapon(int client, int weapon);

void trap_EA_Talk(int client);

void trap_EA_Attack(int client);

void trap_EA_Alt_Attack(int client);

void trap_EA_Use(int client);

void trap_EA_Respawn(int client);

void trap_EA_Jump(int client);

void trap_EA_DelayedJump(int client);

void trap_EA_Crouch(int client);

void trap_EA_MoveUp(int client);

void trap_EA_MoveDown(int client);

void trap_EA_MoveForward(int client);

void trap_EA_MoveBack(int client);

void trap_EA_MoveLeft(int client);

void trap_EA_MoveRight(int client);

void trap_EA_Move(int client, vec3_t dir, float speed);

void trap_EA_View(int client, vec3_t viewangles);

void trap_EA_EndRegular(int client, float thinktime);

void trap_EA_GetInput(int client, float thinktime, void* input);

void trap_EA_ResetInput(int client);

int	trap_BotLoadCharacter(char* charfile, int skill);

void trap_BotFreeCharacter(int character);

float trap_Characteristic_Float(int character, int index);

float trap_Characteristic_BFloat(int character, int index, float min, float max);

int trap_Characteristic_Integer(int character, int index);

int	trap_Characteristic_BInteger(int character, int index, int min, int max);

void trap_Characteristic_String(int character, int index, char *buf, int size);

int	trap_BotAllocChatState(void);

void trap_BotFreeChatState(int handle);

void trap_BotQueueConsoleMessage(int chatstate, int type, char* message);

void trap_BotRemoveConsoleMessage(int chatstate, int handle);

int	trap_BotNextConsoleMessage(int chatstate, void* cm);

int	trap_BotNumConsoleMessages(int chatstate);

void trap_BotInitialChat(int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);

int	trap_BotNumInitialChats(int chatstate, char* type);

int	trap_BotReplyChat(int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);

int trap_BotChatLength(int chatstate);

void trap_BotEnterChat(int chatstate, int client, int sendto);

void trap_BotGetChatMessage(int chatstate, char* buf, int size);

int trap_StringContains(char* str1, char* str2, int casesensitive);

int trap_BotFindMatch(char* str, void* match, unsigned long int context);

void trap_BotMatchVariable(void* match, int variable, char* buf, int size);

void trap_UnifyWhiteSpaces(char* string);

void trap_BotReplaceSynonyms(char* string, unsigned long int context);

int trap_BotLoadChatFile(int chatstate, char* chatfile, char* chatname);

void trap_BotSetChatGender(int chatstate, int gender);

void trap_BotSetChatName(int chatstate, char* name);

void trap_BotResetGoalState(int goalstate);

void trap_BotRemoveFromAvoidGoals(int goalstate, int number);

void trap_BotResetAvoidGoals(int goalstate);

void trap_BotPushGoal(int goalstate, void* goal);

void trap_BotPopGoal(int goalstate);

void trap_BotEmptyGoalStack(int goalstate);

void trap_BotDumpAvoidGoals(int goalstate);

void trap_BotDumpGoalStack(int goalstate);

void trap_BotGoalName(int number, char* name, int size);

int trap_BotGetTopGoal(int goalstate, void* goal);

int	trap_BotGetSecondGoal(int goalstate, void* goal);

int	trap_BotChooseLTGItem(int goalstate, vec3_t origin, int* inventory, int travelflags, qboolean botRoamsOnly);

int	trap_BotChooseNBGItem(int goalstate, vec3_t origin, int* inventory, int travelflags, void* ltg, float maxtime, qboolean botRoamsOnly);

int	trap_BotTouchingGoal(vec3_t origin, void* goal);

int	trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void* goal);

int	trap_BotGetNextCampSpotGoal(int num, void* goal);


int	trap_BotGetMapLocationGoal(char* name, void* goal);

int	trap_BotGetLevelItemGoal(int index, char* classname, void* goal);

float trap_BotAvoidGoalTime(int goalstate, int number);

void trap_BotInitLevelItems(void);

void trap_BotUpdateEntityItems(void);

int	trap_BotLoadItemWeights(int goalstate, char* filename);

void trap_BotFreeItemWeights(int goalstate);

void trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);

void trap_BotSaveGoalFuzzyLogic(int goalstate, char* filename);

void trap_BotMutateGoalFuzzyLogic(int goalstate, float range);

int	 trap_BotAllocGoalState(int state);

void trap_BotFreeGoalState(int handle);

void trap_BotResetMoveState(int movestate);

void trap_BotMoveToGoal(void* result, int movestate, void* goal, int travelflags);

int	trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);

void trap_BotResetAvoidReach(int movestate);

void trap_BotResetLastAvoidReach(int movestate);

int	trap_BotReachabilityArea(vec3_t origin, int testground);

int	trap_BotMovementViewTarget(int movestate, void* goal, int travelflags, float lookahead, vec3_t target);

int	trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void* goal, int travelflags, vec3_t target);

int	trap_BotAllocMoveState(void);

void trap_BotFreeMoveState(int handle);

void trap_BotInitMoveState(int handle, void* initmove);

int	trap_BotChooseBestFightWeapon(int weaponstate, int* inventory, qboolean meleeRange);

void trap_BotGetWeaponInfo(int weaponstate, int weapon, void* weaponinfo);

int	trap_BotLoadWeaponWeights(int weaponstate, char* filename);

int	trap_BotAllocWeaponState(void);

void trap_BotFreeWeaponState(int weaponstate);

void trap_BotResetWeaponState(int weaponstate);

int	trap_GeneticParentsAndChildSelection(int numranks, float* ranks, int* parent1, int* parent2, int* child);

#endif /* _G_SYSCALLS_H */
