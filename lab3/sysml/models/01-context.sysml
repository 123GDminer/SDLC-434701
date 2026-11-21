package GuideModel1 {
part def Visitor; part def Catalog; part def Application { ref part catalog : Catalog; } part def Context { part visitor : Visitor; part guide : Application; part data : Catalog; }
}
