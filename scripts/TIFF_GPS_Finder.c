/* John Williams
  TIFF analyzer to find information as well as GPS location   
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>

struct __attribute__((__packed__)) gps_data{
  unsigned short Type;
  unsigned int count;
  unsigned int offset;
};

struct __attribute__((__packed__)) gps_LatLongRef{
  unsigned short Type;
  unsigned int count;
  char data[4];
};

struct __attribute__((__packed__)) gps_LatLong{
  unsigned short Type;
  unsigned int count;
  unsigned int offset;
};

struct __attribute__((__packed__)) coordinates{
  unsigned int degree_Num;
  unsigned int degree_Den;
  unsigned int minutes_Num;
  unsigned int minutes_Den;
  unsigned int seconds_Num;
  unsigned int seconds_Den;
};

struct __attribute__((__packed__)) Altitude{
  unsigned short Type;
  unsigned int count;
  unsigned int value;
};

struct __attribute__((__packed__)) altitude_info{
  unsigned int Num;
  unsigned int Den;
};

static double compute_Coordinate(struct coordinates *coord,char direction);

uint16_t LE2BE_uint16( uint16_t val ){
  return (val << 8) | (val >> 8);
}

uint32_t LE2BE_uint32( uint32_t val )
{
  val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
  return (val << 16) | (val >> 16);
}

int main(){
  FILE *input;
  char buffer[101];
  char file[100];
  int i=1;
  char old=0;
  short gps_count;
  int gps_info_count;
  short tag;
  struct gps_data *gps=malloc(sizeof(struct gps_data));
  struct gps_LatLong *longitude=malloc(sizeof(struct gps_LatLong));
  struct gps_LatLong *latitude=malloc(sizeof(struct gps_LatLong));
  struct gps_LatLongRef *longitude_ref=malloc(sizeof(struct gps_LatLongRef));
  struct gps_LatLongRef *latitude_ref=malloc(sizeof(struct gps_LatLongRef));
  struct coordinates *Lat_Coord=malloc(sizeof(struct coordinates));
  struct coordinates *Long_Coord=malloc(sizeof(struct coordinates));
  struct Altitude *altitude_ref=malloc(sizeof(struct Altitude));
  struct Altitude *altitude=malloc(sizeof(struct Altitude));
  struct altitude_info *alt_info=malloc(sizeof(struct altitude_info));
  printf("Enter file name.\n");
  scanf("%s",file);
  if(!(input=fopen(file,"r"))){
    printf("error opening file\n");
    exit(1);
  }
  fread(buffer,1,1,input);
  while(!(buffer[0]==(char)0x25&&old==(char)0x88)&&i!=150){
    old=buffer[0];
    fread(buffer,1,1,input);
    i++;
  }
  fread(gps,sizeof(struct gps_data),1,input);
  gps->Type=LE2BE_uint16(gps->Type);
  gps->count=LE2BE_uint32(gps->count);
  gps->offset=LE2BE_uint32(gps->offset);
  i+=10;
  while(i<gps->offset){
    fread(buffer,1,1,input);
    i++;
  }
  fread(&gps_count,sizeof(gps_count),1,input);
  gps_count=LE2BE_uint16(gps_count);
  i+=2;
  for(gps_info_count=0;gps_info_count<gps_count;gps_info_count++){
    fread(&tag,sizeof(tag),1,input);
    tag=LE2BE_uint16(tag);
    i+=2;
    if(tag==1){
      fread(latitude_ref,sizeof(struct gps_LatLongRef),1,input);
      i+=sizeof(struct gps_LatLongRef);
    }else if(tag==2){
      fread(latitude,sizeof(struct gps_LatLong),1,input);
      latitude->offset=LE2BE_uint32(latitude->offset);
      i+=sizeof(struct gps_LatLong);
    }else if(tag==3){
      fread(longitude_ref,sizeof(struct gps_LatLongRef),1,input);
      i+=sizeof(struct gps_LatLongRef);
    }else if(tag==4){
      fread(longitude,sizeof(struct gps_LatLong),1,input);
      longitude->offset=LE2BE_uint32(longitude->offset);
      i+=sizeof(struct gps_LatLong);
    }else if(tag==5){
      fread(altitude_ref,sizeof(struct Altitude),1,input);
      altitude_ref->value=LE2BE_uint32(altitude_ref->value);
      i+=sizeof(struct Altitude);
    }else if(tag==6){
      fread(altitude,sizeof(struct Altitude),1,input);
      altitude->value=LE2BE_uint32(altitude->value);
      i+=sizeof(struct Altitude);
      break;
    }
  }
  while(i<latitude->offset){
    fread(buffer,1,1,input);
    i++;
  }
  fread(Lat_Coord,sizeof(struct coordinates),1,input);
  printf("Latitute is %f\n",compute_Coordinate(Lat_Coord,
        latitude_ref->data[0]));
  fread(Long_Coord,sizeof(struct coordinates),1,input);
  printf("Logitude is %f\n",compute_Coordinate(Long_Coord,
        longitude_ref->data[0]));
  i+=sizeof(struct coordinates)*2;
  while(i<altitude->value){
    fread(buffer,1,1,input);
    i++;
  }
  fread(alt_info,sizeof(struct altitude_info),1,input);
  alt_info->Num=LE2BE_uint32(alt_info->Num);
  alt_info->Den=LE2BE_uint32(alt_info->Den);
  printf("Altitude is %f meters",1.0*alt_info->Num/alt_info->Den);
  if(altitude_ref->value==1){
    printf(" below sea level\n");
  }else{
    printf(" above sea level\n");
  }

  free(gps);
  free(longitude);
  free(latitude);
  free(latitude_ref);
  free(longitude_ref);
  free(Lat_Coord);
  free(Long_Coord);
  free(altitude);
  free(altitude_ref);
  free(alt_info);
  fclose(input);
  return 0;
}

static double compute_Coordinate(struct coordinates *coord,char direction){
  double sum=0;
  coord->degree_Num=LE2BE_uint32(coord->degree_Num);
  coord->degree_Den=LE2BE_uint32(coord->degree_Den);
  coord->minutes_Num=LE2BE_uint32(coord->minutes_Num);
  coord->minutes_Den=LE2BE_uint32(coord->minutes_Den);
  coord->seconds_Num=LE2BE_uint32(coord->seconds_Num);
  coord->seconds_Den=LE2BE_uint32(coord->seconds_Den);

  sum+=(double)coord->degree_Num/coord->degree_Den;
  sum+=(((double)coord->minutes_Num)/coord->minutes_Den)/60.0;
  sum+=(((double)coord->seconds_Num)/coord->seconds_Den)/3600.0;
  if(direction=='S'||direction=='W'){
    sum*=-1;
  }
   
  return sum;
}




