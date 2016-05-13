# LevelDB_API 

Leveldb_API is a library for sharing LevelDB database between multiple processes. Project consists of UNIX-daemon server, which directly communicates with the database, and static library for connecting to server. Project uses [libsocket](https://github.com/AbdullinAM/libsocket.git) library for data transfer.

Library was developed to store serialized objects in DB. So, it only supports `std::string` values as keys and `char*` byte arrays as data.

## API
---------------------
Library API defined in include/DB.hpp file. File contains DB class with following methods: 
* `getInstance()` - get instance of DB class
* `isDaemonStarted()` - check the status of server (started or not)
* `setSocket(const std::string& socket_name)` - configure DB instance to used socket
* `lock()` - lock server
* `unlock()` - unlock server
* `write(const std::string& key, const T& obj)` -write object to DB
* `read(const std::string& key)` - get value from DB
* `read(const std::string& key, Context& ctx)` - same, but when deserialization need context
* `readAll(const std::string& key)` - get all values, which keys starts with "key" string
* `readAll(const std::string& key, Context& ctx)` - same, but when deserialization need context

For the objects, stored in the database, the following methods must be defined:
* `static Buffer serialize(const T &s)` - serialization method, should return serializer::Buffer type object:
```cpp
struct Buffer{
    std::shared_ptr<char> array;
    size_t size;
};
```
* `static auto deserialize(const SerializedT& s, Context& ctx)` or `static auto deserialize(const SerializedT& s)` - deserialization method with or without context (depends on whether object deserialization needs context or not)
* `static auto notFound()` - return value when object not found in DB

## Usage example
------------------
### Starting daemon
------------------
```cpp
if (not leveldb_daemon::DB::isDaemonStarted()) {    //checking is daemon already started
    std::string exePath = getexepath();
    if (fork() == 0) {
        std::string db_name = "/tmp/leveldb-testbase";                          //database name
        std::string socket_name = "/tmp/leveldb-test-server-socket.soc";        //UNIX-socket file name
        std::string lib_path = "lib/leveldb-mp/leveldb_daemon ";
        std::string runCmd = exePath + lib_path + db_name + " " + socket_name;  // starting daemon
        system(runCmd.c_str());
        return 0;
    }
}
auto&& db = leveldb_daemon::DB::getInstance();
db->setSocket("/tmp/leveldb-test-server-socket.soc");   //configuring client to use defined socket
```

### Communicating with DB
------------------
#### Simple write/read
```cpp
std::string key = "my_key";

auto&& db = leveldb_daemon::DB::getInstance();
db->write(key, *data);
auto&& result = db->read<ContractContainer, Context>(key, Cont);
```

### Write/read with locking
```cpp
auto&& db = leveldb_daemon::DB::getInstance();
std::string idKey = "_id";

db->lock();		//locking server to prevent other connections
auto&& idObject = db->read<IdObject, Context>(idKey, Cont);

idObject->change();

db->write(idKey, *idObject);
db->unlock();	//unlocking server
```
