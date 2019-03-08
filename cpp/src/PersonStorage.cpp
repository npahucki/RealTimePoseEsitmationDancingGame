//
// Created by Nathan  Pahucki on 10/24/18.
//

#include <PersonStorage.h>

#include "PersonStorage.h"
#include <memory.h>


u_int32_t PersonStorage::PersonData::PersonDataSize(const PersonStorage::PersonData &personData)
{
    return sizeof(personData.score)
        + sizeof(personData.head)
        + personData.name.length() + 1
        + personData.facePrint.length() + 1;
}

void PersonStorage::PersonData::CopyPersonData(void *dest, const PersonStorage::PersonData &personData)
{
    char *p = (char *)dest;

    const size_t scoreSize = sizeof(personData.score);
    memcpy(p, &personData.score, scoreSize);
    p += scoreSize;

    const size_t headSize = sizeof(personData.head);
    memcpy(p, &personData.head, headSize);
    p += headSize;

    const size_t nameSize = personData.name.length() + 1;
    memcpy(p, personData.name.c_str(), nameSize);
    p += nameSize;

    const size_t facePrintSize = personData.facePrint.length() + 1;
    memcpy(p, personData.facePrint.c_str(), facePrintSize);
}

void PersonStorage::PersonData::RestorePersonData(PersonStorage::PersonData &personData, const void* src)
{
    char *p = (char *) src;

    const size_t scoreSize = sizeof(personData.score);
    memcpy(&personData.score, p, scoreSize);
    p += scoreSize;

    const size_t headSize = sizeof(personData.head);
    memcpy(&personData.head, p, headSize);
    p += headSize;

    personData.name = p;
    p += personData.name.length() + 1;

    personData.facePrint = p;
}


PersonStorage::PersonStorage(const std::string &filePath) {
    db_ = new Db(nullptr, DB_CXX_NO_EXCEPTIONS);
    db_->set_error_stream(&std::cerr);
    db_->open(nullptr, (filePath).c_str(), nullptr, DB_BTREE, DB_CREATE, 0);
    map_ = new PersonDataMap(db_);

    dbstl::DbstlElemTraits<PersonData> *pPersonDataTraits = dbstl::DbstlElemTraits<PersonData>::instance();
    pPersonDataTraits->set_size_function(PersonStorage::PersonData::PersonDataSize);
    pPersonDataTraits->set_copy_function(PersonStorage::PersonData::CopyPersonData);
    pPersonDataTraits->set_restore_function(PersonStorage::PersonData::RestorePersonData);
}

void PersonStorage::storePerson(const std::string &id, const PersonData &data) {
    cache_.erase(id);
    (*map_)[id] = data;
}

const PersonStorage::PersonData PersonStorage::lookupPerson(const std::string &id) {
    if(id.empty()) return emptyPerson_;
    const auto cacheHit = cache_.find(id);
    if(cacheHit != cache_.end()) {
        return (*cacheHit).second;
    }

    const auto data = (*map_)[id];
    cache_[id] = data;
    return data;
}

PersonStorage::PersonDataMap *PersonStorage::getMap() const {
    return map_;
}

void PersonStorage::flush() {
    db_->sync(0);
}


