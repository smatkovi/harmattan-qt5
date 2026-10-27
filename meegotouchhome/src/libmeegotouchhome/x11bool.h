/*
 * Qt 5: Xlib's "#define Bool int" turns QMetaType::Bool in the code moc
 * writes into QMetaType::int (as in mcompositor).  Included after the X
 * headers in the headers moc reads: Bool becomes a type instead of a
 * macro, so both work.
 */
#ifdef Bool
#undef Bool
typedef int Bool;
#endif
