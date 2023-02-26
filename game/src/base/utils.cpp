///////////////////////////////////////////////////////////
//// NOTE(Elias): Files

internal S32
filelen(FILE *file_connection)
{
  S32 l = 0;
  fseek(file_connection, 0, SEEK_END);
  l = ftell(file_connection);
  fseek(file_connection, 0, 0);
  return(l);
}
