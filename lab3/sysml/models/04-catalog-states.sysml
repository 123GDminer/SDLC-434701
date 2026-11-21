package GuideModel3 {
state def CatalogLifecycle {
 entry; then loading;
 state loading; state ready; state empty; state failed;
 transition first loading accept RecordsFound then ready;
 transition first loading accept NoRecords then empty;
 transition first loading accept ReadError then failed;
}
attribute def RecordsFound; attribute def NoRecords; attribute def ReadError;
}
