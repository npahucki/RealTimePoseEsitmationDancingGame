//
// Created by Nathan  Pahucki on 10/24/18.
//

#ifndef GESTURES_PERSONSTORAGE_H
#define GESTURES_PERSONSTORAGE_H

#include <db_cxx.h>
#include <dbstl_map.h>
#include <string>
#include <dlib/matrix.h>


class PersonStorage {

public:

    class PersonData {
    public:
        int score = -1;
        int head = -1;
        std::string name = "";
        std::string facePrint = "";

        static u_int32_t PersonDataSize(const PersonStorage::PersonData &personData);
        static void CopyPersonData(void *dest, const PersonStorage::PersonData &personData);
        static void RestorePersonData(PersonStorage::PersonData &personData, const void* src);


    };

    typedef dbstl::db_map<std::string, PersonData> PersonDataMap;
    typedef std::map<std::string, PersonData> PersonDataCache;

    PersonStorage(const std::string &filePath);

    /**
     * TODO: Remove this, this is a clear viloation of encapulation of this class, but I
     * can't find a good way to iterate this thing without exposing it right now.
     */
    PersonDataMap *getMap() const;

    ~PersonStorage() {
        delete map_;
        delete db_; // deleting closes it.
    }

    void storePerson(const std::string &id, const PersonData &data);

    const PersonData lookupPerson(const std::string &id);

    void flush();

private:
    Db *db_ = nullptr;
    PersonDataMap *map_ = nullptr;
    PersonDataCache cache_;
    const PersonData emptyPerson_;
};


#endif //GESTURES_PERSONSTORAGE_H
