---@meta

---@class StatsData
---@field hp     integer
---@field dmg    integer
---@field atk    integer
---@field def    integer
---@field speed  integer

---@class TileData
---@field textureIndex0  integer   -- -1 = no texture
---@field textureIndex1  integer   -- -1 = no texture
---@field tileClipX      integer
---@field tileClipY      integer
---@field isCollider     boolean
---@field isLightSource  boolean
---@field isLevelExit    boolean
---@field isLethal       boolean
---@field lockType       integer   -- 0=none, 1=gold, 2=silver

---@class TrapData
---@field texturePath1 string
---@field texturePath2 string
---@field clipX        integer
---@field clipY        integer
---@field damage       integer

---@class ChestData
---@field texturePath1 string
---@field texturePath2 string
---@field clipX        integer
---@field clipY        integer

---@class MonsterData
---@field label        string
---@field texturePath1 string
---@field texturePath2 string
---@field stats        StatsData
---@field clipX        integer
---@field clipY        integer
---@field behaviour    integer
---@field boss         boolean

---@class PlayerDataTable
---@field shopOwnerKiller boolean
---@field level           integer

---@alias Map userdata          -- opaque C lightuserdata

---@alias RoomModifier  "WINDY"|"FIRE"|"CRUMBLING"
---@alias Direction     "LEFT"|"RIGHT"|"UP"|"DOWN"

---@type Map
map = nil

---@type integer
CURRENT_LEVEL = nil

---@type boolean
QUICK_MODE = nil

---@type boolean
ARCADE_MODE = nil

---@type PlayerDataTable
PlayerData = nil

--- Create a new map for the given dungeon level.
---@param level integer
---@return Map
function create_map(level) end

--- Print an info message to the game log.
---@param msg string
function info(msg) end

--- Add a floor tile to the map.
---@param m Map
---@param x integer
---@param y integer
---@param tile TileData
function add_tile(m, x, y, tile) end

--- Add a wall tile to the map.
---@param m Map
---@param x integer
---@param y integer
---@param tile TileData
function add_wall(m, x, y, tile) end

--- Add a door tile to the map.
---@param m Map
---@param x integer
---@param y integer
---@param tile TileData
function add_door(m, x, y, tile) end

--- Add a decoration tile to the map.
---@param m Map
---@param x integer
---@param y integer
---@param tile TileData
function add_decoration(m, x, y, tile) end

--- Add a trap to the map.
---@param m Map
---@param x integer
---@param y integer
---@param trap TrapData
function add_trap(m, x, y, trap) end

--- Add a chest to the map.
---@param m Map
---@param x integer
---@param y integer
---@param level integer
---@param chest ChestData
function add_chest(m, x, y, level, chest) end

--- Load a texture and return its index for use in TileData fields.
---@param m Map
---@param path string
---@return integer
function add_texture(m, path) end

--- Read the contents of a file relative to the data directory.
---@param filename string
---@return string
function read_file(filename) end

--- Set the current room origin for subsequent monster/object placement.
---@param m Map
---@param x integer
---@param y integer
function set_current_room(m, x, y) end

--- Apply a modifier to the current room.
---@param m Map
---@param modifier RoomModifier
---@param direction Direction
function set_modifier(m, modifier, direction) end

--- Add a monster to the map.
---@param m Map
---@param x integer
---@param y integer
---@param monster MonsterData
function add_monster(m, x, y, monster) end

--- Assign a key to a random monster already on the map.
---@param m Map
function add_keybearers(m) end

--- Get the seeded random value for this dungeon level (used to seed math.random).
---@param level integer
---@return number
function get_random_seed(level) end

--- Seed the map's internal PRNG.
---@param seed integer
function map_randomseed(seed) end

--- Return a random integer in the range [1, max].
---@param max integer
---@return integer
function map_random(max) end

--- Place a shop artifact at the given position.
---@param m Map
---@param x integer
---@param y integer
function create_shop_artifact(m, x, y) end
