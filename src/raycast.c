#include "raycast.h"
#include "framebuffer.h"
#include "config.h"
#include "maths.h"
#include <math.h>
#include <stdbool.h>

// Setting some fix32 zero results to 0.01 to avoid divide-by-zero errors
const fix32 sinLookup[ 360 ] = { FIX32(0.01), FIX32(0.0174524064373), FIX32(0.0348994967025), FIX32(0.0523359562429), FIX32(0.0697564737441), FIX32(0.0871557427477), FIX32(0.104528463268), FIX32(0.121869343405), FIX32(0.13917310096), FIX32(0.15643446504), FIX32(0.173648177667), FIX32(0.190808995377), FIX32(0.207911690818), FIX32(0.224951054344), FIX32(0.2419218956), FIX32(0.258819045103), FIX32(0.275637355817), FIX32(0.292371704723), FIX32(0.309016994375), FIX32(0.325568154457), FIX32(0.342020143326), FIX32(0.358367949545), FIX32(0.374606593416), FIX32(0.390731128489), FIX32(0.406736643076), FIX32(0.422618261741), FIX32(0.438371146789), FIX32(0.45399049974), FIX32(0.469471562786), FIX32(0.484809620246), FIX32(0.5), FIX32(0.51503807491), FIX32(0.529919264233), FIX32(0.544639035015), FIX32(0.559192903471), FIX32(0.573576436351), FIX32(0.587785252292), FIX32(0.601815023152), FIX32(0.615661475326), FIX32(0.62932039105), FIX32(0.642787609687), FIX32(0.656059028991), FIX32(0.669130606359), FIX32(0.681998360062), FIX32(0.694658370459), FIX32(0.707106781187), FIX32(0.719339800339), FIX32(0.731353701619), FIX32(0.743144825477), FIX32(0.754709580223), FIX32(0.766044443119), FIX32(0.777145961457), FIX32(0.788010753607), FIX32(0.798635510047), FIX32(0.809016994375), FIX32(0.819152044289), FIX32(0.829037572555), FIX32(0.838670567945), FIX32(0.848048096156), FIX32(0.857167300702), FIX32(0.866025403784), FIX32(0.874619707139), FIX32(0.882947592859), FIX32(0.891006524188), FIX32(0.898794046299), FIX32(0.906307787037), FIX32(0.913545457643), FIX32(0.920504853452), FIX32(0.927183854567), FIX32(0.933580426497), FIX32(0.939692620786), FIX32(0.945518575599), FIX32(0.951056516295), FIX32(0.956304755963), FIX32(0.961261695938), FIX32(0.965925826289), FIX32(0.970295726276), FIX32(0.974370064785), FIX32(0.978147600734), FIX32(0.981627183448), FIX32(0.984807753012), FIX32(0.987688340595), FIX32(0.990268068742), FIX32(0.992546151641), FIX32(0.994521895368), FIX32(0.996194698092), FIX32(0.99756405026), FIX32(0.998629534755), FIX32(0.999390827019), FIX32(0.999847695156), FIX32(1.0), FIX32(0.999847695156), FIX32(0.999390827019), FIX32(0.998629534755), FIX32(0.99756405026), FIX32(0.996194698092), FIX32(0.994521895368), FIX32(0.992546151641), FIX32(0.990268068742), FIX32(0.987688340595), FIX32(0.984807753012), FIX32(0.981627183448), FIX32(0.978147600734), FIX32(0.974370064785), FIX32(0.970295726276), FIX32(0.965925826289), FIX32(0.961261695938), FIX32(0.956304755963), FIX32(0.951056516295), FIX32(0.945518575599), FIX32(0.939692620786), FIX32(0.933580426497), FIX32(0.927183854567), FIX32(0.920504853452), FIX32(0.913545457643), FIX32(0.906307787037), FIX32(0.898794046299), FIX32(0.891006524188), FIX32(0.882947592859), FIX32(0.874619707139), FIX32(0.866025403784), FIX32(0.857167300702), FIX32(0.848048096156), FIX32(0.838670567945), FIX32(0.829037572555), FIX32(0.819152044289), FIX32(0.809016994375), FIX32(0.798635510047), FIX32(0.788010753607), FIX32(0.777145961457), FIX32(0.766044443119), FIX32(0.754709580223), FIX32(0.743144825477), FIX32(0.731353701619), FIX32(0.719339800339), FIX32(0.707106781187), FIX32(0.694658370459), FIX32(0.681998360062), FIX32(0.669130606359), FIX32(0.656059028991), FIX32(0.642787609687), FIX32(0.62932039105), FIX32(0.615661475326), FIX32(0.601815023152), FIX32(0.587785252292), FIX32(0.573576436351), FIX32(0.559192903471), FIX32(0.544639035015), FIX32(0.529919264233), FIX32(0.51503807491), FIX32(0.5), FIX32(0.484809620246), FIX32(0.469471562786), FIX32(0.45399049974), FIX32(0.438371146789), FIX32(0.422618261741), FIX32(0.406736643076), FIX32(0.390731128489), FIX32(0.374606593416), FIX32(0.358367949545), FIX32(0.342020143326), FIX32(0.325568154457), FIX32(0.309016994375), FIX32(0.292371704723), FIX32(0.275637355817), FIX32(0.258819045103), FIX32(0.2419218956), FIX32(0.224951054344), FIX32(0.207911690818), FIX32(0.190808995377), FIX32(0.173648177667), FIX32(0.15643446504), FIX32(0.13917310096), FIX32(0.121869343405), FIX32(0.104528463268), FIX32(0.0871557427477), FIX32(0.0697564737441), FIX32(0.0523359562429), FIX32(0.0348994967025), FIX32(0.0174524064373), FIX32(1.22464679915e-16), FIX32(-0.0174524064373), FIX32(-0.0348994967025), FIX32(-0.0523359562429), FIX32(-0.0697564737441), FIX32(-0.0871557427477), FIX32(-0.104528463268), FIX32(-0.121869343405), FIX32(-0.13917310096), FIX32(-0.15643446504), FIX32(-0.173648177667), FIX32(-0.190808995377), FIX32(-0.207911690818), FIX32(-0.224951054344), FIX32(-0.2419218956), FIX32(-0.258819045103), FIX32(-0.275637355817), FIX32(-0.292371704723), FIX32(-0.309016994375), FIX32(-0.325568154457), FIX32(-0.342020143326), FIX32(-0.358367949545), FIX32(-0.374606593416), FIX32(-0.390731128489), FIX32(-0.406736643076), FIX32(-0.422618261741), FIX32(-0.438371146789), FIX32(-0.45399049974), FIX32(-0.469471562786), FIX32(-0.484809620246), FIX32(-0.5), FIX32(-0.51503807491), FIX32(-0.529919264233), FIX32(-0.544639035015), FIX32(-0.559192903471), FIX32(-0.573576436351), FIX32(-0.587785252292), FIX32(-0.601815023152), FIX32(-0.615661475326), FIX32(-0.62932039105), FIX32(-0.642787609687), FIX32(-0.656059028991), FIX32(-0.669130606359), FIX32(-0.681998360062), FIX32(-0.694658370459), FIX32(-0.707106781187), FIX32(-0.719339800339), FIX32(-0.731353701619), FIX32(-0.743144825477), FIX32(-0.754709580223), FIX32(-0.766044443119), FIX32(-0.777145961457), FIX32(-0.788010753607), FIX32(-0.798635510047), FIX32(-0.809016994375), FIX32(-0.819152044289), FIX32(-0.829037572555), FIX32(-0.838670567945), FIX32(-0.848048096156), FIX32(-0.857167300702), FIX32(-0.866025403784), FIX32(-0.874619707139), FIX32(-0.882947592859), FIX32(-0.891006524188), FIX32(-0.898794046299), FIX32(-0.906307787037), FIX32(-0.913545457643), FIX32(-0.920504853452), FIX32(-0.927183854567), FIX32(-0.933580426497), FIX32(-0.939692620786), FIX32(-0.945518575599), FIX32(-0.951056516295), FIX32(-0.956304755963), FIX32(-0.961261695938), FIX32(-0.965925826289), FIX32(-0.970295726276), FIX32(-0.974370064785), FIX32(-0.978147600734), FIX32(-0.981627183448), FIX32(-0.984807753012), FIX32(-0.987688340595), FIX32(-0.990268068742), FIX32(-0.992546151641), FIX32(-0.994521895368), FIX32(-0.996194698092), FIX32(-0.99756405026), FIX32(-0.998629534755), FIX32(-0.999390827019), FIX32(-0.999847695156), FIX32(-1.0), FIX32(-0.999847695156), FIX32(-0.999390827019), FIX32(-0.998629534755), FIX32(-0.99756405026), FIX32(-0.996194698092), FIX32(-0.994521895368), FIX32(-0.992546151641), FIX32(-0.990268068742), FIX32(-0.987688340595), FIX32(-0.984807753012), FIX32(-0.981627183448), FIX32(-0.978147600734), FIX32(-0.974370064785), FIX32(-0.970295726276), FIX32(-0.965925826289), FIX32(-0.961261695938), FIX32(-0.956304755963), FIX32(-0.951056516295), FIX32(-0.945518575599), FIX32(-0.939692620786), FIX32(-0.933580426497), FIX32(-0.927183854567), FIX32(-0.920504853452), FIX32(-0.913545457643), FIX32(-0.906307787037), FIX32(-0.898794046299), FIX32(-0.891006524188), FIX32(-0.882947592859), FIX32(-0.874619707139), FIX32(-0.866025403784), FIX32(-0.857167300702), FIX32(-0.848048096156), FIX32(-0.838670567945), FIX32(-0.829037572555), FIX32(-0.819152044289), FIX32(-0.809016994375), FIX32(-0.798635510047), FIX32(-0.788010753607), FIX32(-0.777145961457), FIX32(-0.766044443119), FIX32(-0.754709580223), FIX32(-0.743144825477), FIX32(-0.731353701619), FIX32(-0.719339800339), FIX32(-0.707106781187), FIX32(-0.694658370459), FIX32(-0.681998360062), FIX32(-0.669130606359), FIX32(-0.656059028991), FIX32(-0.642787609687), FIX32(-0.62932039105), FIX32(-0.615661475326), FIX32(-0.601815023152), FIX32(-0.587785252292), FIX32(-0.573576436351), FIX32(-0.559192903471), FIX32(-0.544639035015), FIX32(-0.529919264233), FIX32(-0.51503807491), FIX32(-0.5), FIX32(-0.484809620246), FIX32(-0.469471562786), FIX32(-0.45399049974), FIX32(-0.438371146789), FIX32(-0.422618261741), FIX32(-0.406736643076), FIX32(-0.390731128489), FIX32(-0.374606593416), FIX32(-0.358367949545), FIX32(-0.342020143326), FIX32(-0.325568154457), FIX32(-0.309016994375), FIX32(-0.292371704723), FIX32(-0.275637355817), FIX32(-0.258819045103), FIX32(-0.2419218956), FIX32(-0.224951054344), FIX32(-0.207911690818), FIX32(-0.190808995377), FIX32(-0.173648177667), FIX32(-0.15643446504), FIX32(-0.13917310096), FIX32(-0.121869343405), FIX32(-0.104528463268), FIX32(-0.0871557427477), FIX32(-0.0697564737441), FIX32(-0.0523359562429), FIX32(-0.0348994967025), FIX32(-0.0174524064373) };
const fix32 cosLookup[ 360 ] = { FIX32(1.0), FIX32(0.999847695156), FIX32(0.999390827019), FIX32(0.998629534755), FIX32(0.99756405026), FIX32(0.996194698092), FIX32(0.994521895368), FIX32(0.992546151641), FIX32(0.990268068742), FIX32(0.987688340595), FIX32(0.984807753012), FIX32(0.981627183448), FIX32(0.978147600734), FIX32(0.974370064785), FIX32(0.970295726276), FIX32(0.965925826289), FIX32(0.961261695938), FIX32(0.956304755963), FIX32(0.951056516295), FIX32(0.945518575599), FIX32(0.939692620786), FIX32(0.933580426497), FIX32(0.927183854567), FIX32(0.920504853452), FIX32(0.913545457643), FIX32(0.906307787037), FIX32(0.898794046299), FIX32(0.891006524188), FIX32(0.882947592859), FIX32(0.874619707139), FIX32(0.866025403784), FIX32(0.857167300702), FIX32(0.848048096156), FIX32(0.838670567945), FIX32(0.829037572555), FIX32(0.819152044289), FIX32(0.809016994375), FIX32(0.798635510047), FIX32(0.788010753607), FIX32(0.777145961457), FIX32(0.766044443119), FIX32(0.754709580223), FIX32(0.743144825477), FIX32(0.731353701619), FIX32(0.719339800339), FIX32(0.707106781187), FIX32(0.694658370459), FIX32(0.681998360062), FIX32(0.669130606359), FIX32(0.656059028991), FIX32(0.642787609687), FIX32(0.62932039105), FIX32(0.615661475326), FIX32(0.601815023152), FIX32(0.587785252292), FIX32(0.573576436351), FIX32(0.559192903471), FIX32(0.544639035015), FIX32(0.529919264233), FIX32(0.51503807491), FIX32(0.5), FIX32(0.484809620246), FIX32(0.469471562786), FIX32(0.45399049974), FIX32(0.438371146789), FIX32(0.422618261741), FIX32(0.406736643076), FIX32(0.390731128489), FIX32(0.374606593416), FIX32(0.358367949545), FIX32(0.342020143326), FIX32(0.325568154457), FIX32(0.309016994375), FIX32(0.292371704723), FIX32(0.275637355817), FIX32(0.258819045103), FIX32(0.2419218956), FIX32(0.224951054344), FIX32(0.207911690818), FIX32(0.190808995377), FIX32(0.173648177667), FIX32(0.15643446504), FIX32(0.13917310096), FIX32(0.121869343405), FIX32(0.104528463268), FIX32(0.0871557427477), FIX32(0.0697564737441), FIX32(0.0523359562429), FIX32(0.0348994967025), FIX32(0.0174524064373), FIX32(6.12323399574e-17), FIX32(-0.0174524064373), FIX32(-0.0348994967025), FIX32(-0.0523359562429), FIX32(-0.0697564737441), FIX32(-0.0871557427477), FIX32(-0.104528463268), FIX32(-0.121869343405), FIX32(-0.13917310096), FIX32(-0.15643446504), FIX32(-0.173648177667), FIX32(-0.190808995377), FIX32(-0.207911690818), FIX32(-0.224951054344), FIX32(-0.2419218956), FIX32(-0.258819045103), FIX32(-0.275637355817), FIX32(-0.292371704723), FIX32(-0.309016994375), FIX32(-0.325568154457), FIX32(-0.342020143326), FIX32(-0.358367949545), FIX32(-0.374606593416), FIX32(-0.390731128489), FIX32(-0.406736643076), FIX32(-0.422618261741), FIX32(-0.438371146789), FIX32(-0.45399049974), FIX32(-0.469471562786), FIX32(-0.484809620246), FIX32(-0.5), FIX32(-0.51503807491), FIX32(-0.529919264233), FIX32(-0.544639035015), FIX32(-0.559192903471), FIX32(-0.573576436351), FIX32(-0.587785252292), FIX32(-0.601815023152), FIX32(-0.615661475326), FIX32(-0.62932039105), FIX32(-0.642787609687), FIX32(-0.656059028991), FIX32(-0.669130606359), FIX32(-0.681998360062), FIX32(-0.694658370459), FIX32(-0.707106781187), FIX32(-0.719339800339), FIX32(-0.731353701619), FIX32(-0.743144825477), FIX32(-0.754709580223), FIX32(-0.766044443119), FIX32(-0.777145961457), FIX32(-0.788010753607), FIX32(-0.798635510047), FIX32(-0.809016994375), FIX32(-0.819152044289), FIX32(-0.829037572555), FIX32(-0.838670567945), FIX32(-0.848048096156), FIX32(-0.857167300702), FIX32(-0.866025403784), FIX32(-0.874619707139), FIX32(-0.882947592859), FIX32(-0.891006524188), FIX32(-0.898794046299), FIX32(-0.906307787037), FIX32(-0.913545457643), FIX32(-0.920504853452), FIX32(-0.927183854567), FIX32(-0.933580426497), FIX32(-0.939692620786), FIX32(-0.945518575599), FIX32(-0.951056516295), FIX32(-0.956304755963), FIX32(-0.961261695938), FIX32(-0.965925826289), FIX32(-0.970295726276), FIX32(-0.974370064785), FIX32(-0.978147600734), FIX32(-0.981627183448), FIX32(-0.984807753012), FIX32(-0.987688340595), FIX32(-0.990268068742), FIX32(-0.992546151641), FIX32(-0.994521895368), FIX32(-0.996194698092), FIX32(-0.99756405026), FIX32(-0.998629534755), FIX32(-0.999390827019), FIX32(-0.999847695156), FIX32(-1.0), FIX32(-0.999847695156), FIX32(-0.999390827019), FIX32(-0.998629534755), FIX32(-0.99756405026), FIX32(-0.996194698092), FIX32(-0.994521895368), FIX32(-0.992546151641), FIX32(-0.990268068742), FIX32(-0.987688340595), FIX32(-0.984807753012), FIX32(-0.981627183448), FIX32(-0.978147600734), FIX32(-0.974370064785), FIX32(-0.970295726276), FIX32(-0.965925826289), FIX32(-0.961261695938), FIX32(-0.956304755963), FIX32(-0.951056516295), FIX32(-0.945518575599), FIX32(-0.939692620786), FIX32(-0.933580426497), FIX32(-0.927183854567), FIX32(-0.920504853452), FIX32(-0.913545457643), FIX32(-0.906307787037), FIX32(-0.898794046299), FIX32(-0.891006524188), FIX32(-0.882947592859), FIX32(-0.874619707139), FIX32(-0.866025403784), FIX32(-0.857167300702), FIX32(-0.848048096156), FIX32(-0.838670567945), FIX32(-0.829037572555), FIX32(-0.819152044289), FIX32(-0.809016994375), FIX32(-0.798635510047), FIX32(-0.788010753607), FIX32(-0.777145961457), FIX32(-0.766044443119), FIX32(-0.754709580223), FIX32(-0.743144825477), FIX32(-0.731353701619), FIX32(-0.719339800339), FIX32(-0.707106781187), FIX32(-0.694658370459), FIX32(-0.681998360062), FIX32(-0.669130606359), FIX32(-0.656059028991), FIX32(-0.642787609687), FIX32(-0.62932039105), FIX32(-0.615661475326), FIX32(-0.601815023152), FIX32(-0.587785252292), FIX32(-0.573576436351), FIX32(-0.559192903471), FIX32(-0.544639035015), FIX32(-0.529919264233), FIX32(-0.51503807491), FIX32(-0.5), FIX32(-0.484809620246), FIX32(-0.469471562786), FIX32(-0.45399049974), FIX32(-0.438371146789), FIX32(-0.422618261741), FIX32(-0.406736643076), FIX32(-0.390731128489), FIX32(-0.374606593416), FIX32(-0.358367949545), FIX32(-0.342020143326), FIX32(-0.325568154457), FIX32(-0.309016994375), FIX32(-0.292371704723), FIX32(-0.275637355817), FIX32(-0.258819045103), FIX32(-0.2419218956), FIX32(-0.224951054344), FIX32(-0.207911690818), FIX32(-0.190808995377), FIX32(-0.173648177667), FIX32(-0.15643446504), FIX32(-0.13917310096), FIX32(-0.121869343405), FIX32(-0.104528463268), FIX32(-0.0871557427477), FIX32(-0.0697564737441), FIX32(-0.0523359562429), FIX32(-0.0348994967025), FIX32(-0.0174524064373), FIX32(-1.83697019872e-16), FIX32(0.0174524064373), FIX32(0.0348994967025), FIX32(0.0523359562429), FIX32(0.0697564737441), FIX32(0.0871557427477), FIX32(0.104528463268), FIX32(0.121869343405), FIX32(0.13917310096), FIX32(0.15643446504), FIX32(0.173648177667), FIX32(0.190808995377), FIX32(0.207911690818), FIX32(0.224951054344), FIX32(0.2419218956), FIX32(0.258819045103), FIX32(0.275637355817), FIX32(0.292371704723), FIX32(0.309016994375), FIX32(0.325568154457), FIX32(0.342020143326), FIX32(0.358367949545), FIX32(0.374606593416), FIX32(0.390731128489), FIX32(0.406736643076), FIX32(0.422618261741), FIX32(0.438371146789), FIX32(0.45399049974), FIX32(0.469471562786), FIX32(0.484809620246), FIX32(0.5), FIX32(0.51503807491), FIX32(0.529919264233), FIX32(0.544639035015), FIX32(0.559192903471), FIX32(0.573576436351), FIX32(0.587785252292), FIX32(0.601815023152), FIX32(0.615661475326), FIX32(0.62932039105), FIX32(0.642787609687), FIX32(0.656059028991), FIX32(0.669130606359), FIX32(0.681998360062), FIX32(0.694658370459), FIX32(0.707106781187), FIX32(0.719339800339), FIX32(0.731353701619), FIX32(0.743144825477), FIX32(0.754709580223), FIX32(0.766044443119), FIX32(0.777145961457), FIX32(0.788010753607), FIX32(0.798635510047), FIX32(0.809016994375), FIX32(0.819152044289), FIX32(0.829037572555), FIX32(0.838670567945), FIX32(0.848048096156), FIX32(0.857167300702), FIX32(0.866025403784), FIX32(0.874619707139), FIX32(0.882947592859), FIX32(0.891006524188), FIX32(0.898794046299), FIX32(0.906307787037), FIX32(0.913545457643), FIX32(0.920504853452), FIX32(0.927183854567), FIX32(0.933580426497), FIX32(0.939692620786), FIX32(0.945518575599), FIX32(0.951056516295), FIX32(0.956304755963), FIX32(0.961261695938), FIX32(0.965925826289), FIX32(0.970295726276), FIX32(0.974370064785), FIX32(0.978147600734), FIX32(0.981627183448), FIX32(0.984807753012), FIX32(0.987688340595), FIX32(0.990268068742), FIX32(0.992546151641), FIX32(0.994521895368), FIX32(0.996194698092), FIX32(0.99756405026), FIX32(0.998629534755), FIX32(0.999390827019), FIX32(0.999847695156) };
const fix32 tanLookup[ 360 ] = { FIX32(0.01), FIX32(0.0174550649282), FIX32(0.0349207694917), FIX32(0.052407779283), FIX32(0.0699268119435), FIX32(0.0874886635259), FIX32(0.105104235266), FIX32(0.122784560903), FIX32(0.140540834702), FIX32(0.158384440325), FIX32(0.176326980708), FIX32(0.194380309138), FIX32(0.21255656167), FIX32(0.230868191126), FIX32(0.249328002843), FIX32(0.267949192431), FIX32(0.286745385759), FIX32(0.305730681459), FIX32(0.324919696233), FIX32(0.34432761329), FIX32(0.363970234266), FIX32(0.383864035035), FIX32(0.404026225835), FIX32(0.42447481621), FIX32(0.445228685309), FIX32(0.466307658155), FIX32(0.487732588566), FIX32(0.509525449494), FIX32(0.531709431661), FIX32(0.554309051453), FIX32(0.57735026919), FIX32(0.600860619028), FIX32(0.624869351909), FIX32(0.649407593198), FIX32(0.674508516842), FIX32(0.70020753821), FIX32(0.726542528005), FIX32(0.753554050103), FIX32(0.781285626507), FIX32(0.809784033195), FIX32(0.839099631177), FIX32(0.869286737816), FIX32(0.900404044298), FIX32(0.932515086138), FIX32(0.965688774807), FIX32(1.0), FIX32(1.03553031379), FIX32(1.07236871002), FIX32(1.11061251483), FIX32(1.15036840722), FIX32(1.19175359259), FIX32(1.23489715654), FIX32(1.27994163219), FIX32(1.32704482162), FIX32(1.37638192047), FIX32(1.42814800674), FIX32(1.48256096851), FIX32(1.53986496381), FIX32(1.60033452904), FIX32(1.66427948235), FIX32(1.73205080757), FIX32(1.80404775527), FIX32(1.88072646535), FIX32(1.96261050551), FIX32(2.05030384158), FIX32(2.14450692051), FIX32(2.2460367739), FIX32(2.35585236582), FIX32(2.47508685342), FIX32(2.60508906469), FIX32(2.74747741945), FIX32(2.90421087768), FIX32(3.07768353718), FIX32(3.27085261848), FIX32(3.48741444384), FIX32(3.73205080757), FIX32(4.01078093354), FIX32(4.33147587428), FIX32(4.70463010948), FIX32(5.14455401597), FIX32(5.67128181962), FIX32(6.31375151468), FIX32(7.11536972238), FIX32(8.14434642797), FIX32(9.51436445422), FIX32(11.4300523028), FIX32(14.3006662567), FIX32(19.0811366877), FIX32(28.6362532829), FIX32(57.2899616308), FIX32(1.63312393532e+16), FIX32(-57.2899616308), FIX32(-28.6362532829), FIX32(-19.0811366877), FIX32(-14.3006662567), FIX32(-11.4300523028), FIX32(-9.51436445422), FIX32(-8.14434642797), FIX32(-7.11536972238), FIX32(-6.31375151468), FIX32(-5.67128181962), FIX32(-5.14455401597), FIX32(-4.70463010948), FIX32(-4.33147587428), FIX32(-4.01078093354), FIX32(-3.73205080757), FIX32(-3.48741444384), FIX32(-3.27085261848), FIX32(-3.07768353718), FIX32(-2.90421087768), FIX32(-2.74747741945), FIX32(-2.60508906469), FIX32(-2.47508685342), FIX32(-2.35585236582), FIX32(-2.2460367739), FIX32(-2.14450692051), FIX32(-2.05030384158), FIX32(-1.96261050551), FIX32(-1.88072646535), FIX32(-1.80404775527), FIX32(-1.73205080757), FIX32(-1.66427948235), FIX32(-1.60033452904), FIX32(-1.53986496381), FIX32(-1.48256096851), FIX32(-1.42814800674), FIX32(-1.37638192047), FIX32(-1.32704482162), FIX32(-1.27994163219), FIX32(-1.23489715654), FIX32(-1.19175359259), FIX32(-1.15036840722), FIX32(-1.11061251483), FIX32(-1.07236871002), FIX32(-1.03553031379), FIX32(-1.0), FIX32(-0.965688774807), FIX32(-0.932515086138), FIX32(-0.900404044298), FIX32(-0.869286737816), FIX32(-0.839099631177), FIX32(-0.809784033195), FIX32(-0.781285626507), FIX32(-0.753554050103), FIX32(-0.726542528005), FIX32(-0.70020753821), FIX32(-0.674508516842), FIX32(-0.649407593198), FIX32(-0.624869351909), FIX32(-0.600860619028), FIX32(-0.57735026919), FIX32(-0.554309051453), FIX32(-0.531709431661), FIX32(-0.509525449494), FIX32(-0.487732588566), FIX32(-0.466307658155), FIX32(-0.445228685309), FIX32(-0.42447481621), FIX32(-0.404026225835), FIX32(-0.383864035035), FIX32(-0.363970234266), FIX32(-0.34432761329), FIX32(-0.324919696233), FIX32(-0.305730681459), FIX32(-0.286745385759), FIX32(-0.267949192431), FIX32(-0.249328002843), FIX32(-0.230868191126), FIX32(-0.21255656167), FIX32(-0.194380309138), FIX32(-0.176326980708), FIX32(-0.158384440325), FIX32(-0.140540834702), FIX32(-0.122784560903), FIX32(-0.105104235266), FIX32(-0.0874886635259), FIX32(-0.0699268119435), FIX32(-0.052407779283), FIX32(-0.0349207694917), FIX32(-0.0174550649282), FIX32(-1.22464679915e-16), FIX32(0.0174550649282), FIX32(0.0349207694917), FIX32(0.052407779283), FIX32(0.0699268119435), FIX32(0.0874886635259), FIX32(0.105104235266), FIX32(0.122784560903), FIX32(0.140540834702), FIX32(0.158384440325), FIX32(0.176326980708), FIX32(0.194380309138), FIX32(0.21255656167), FIX32(0.230868191126), FIX32(0.249328002843), FIX32(0.267949192431), FIX32(0.286745385759), FIX32(0.305730681459), FIX32(0.324919696233), FIX32(0.34432761329), FIX32(0.363970234266), FIX32(0.383864035035), FIX32(0.404026225835), FIX32(0.42447481621), FIX32(0.445228685309), FIX32(0.466307658155), FIX32(0.487732588566), FIX32(0.509525449494), FIX32(0.531709431661), FIX32(0.554309051453), FIX32(0.57735026919), FIX32(0.600860619028), FIX32(0.624869351909), FIX32(0.649407593198), FIX32(0.674508516842), FIX32(0.70020753821), FIX32(0.726542528005), FIX32(0.753554050103), FIX32(0.781285626507), FIX32(0.809784033195), FIX32(0.839099631177), FIX32(0.869286737816), FIX32(0.900404044298), FIX32(0.932515086138), FIX32(0.965688774807), FIX32(1.0), FIX32(1.03553031379), FIX32(1.07236871002), FIX32(1.11061251483), FIX32(1.15036840722), FIX32(1.19175359259), FIX32(1.23489715654), FIX32(1.27994163219), FIX32(1.32704482162), FIX32(1.37638192047), FIX32(1.42814800674), FIX32(1.48256096851), FIX32(1.53986496381), FIX32(1.60033452904), FIX32(1.66427948235), FIX32(1.73205080757), FIX32(1.80404775527), FIX32(1.88072646535), FIX32(1.96261050551), FIX32(2.05030384158), FIX32(2.14450692051), FIX32(2.2460367739), FIX32(2.35585236582), FIX32(2.47508685342), FIX32(2.60508906469), FIX32(2.74747741945), FIX32(2.90421087768), FIX32(3.07768353718), FIX32(3.27085261848), FIX32(3.48741444384), FIX32(3.73205080757), FIX32(4.01078093354), FIX32(4.33147587428), FIX32(4.70463010948), FIX32(5.14455401597), FIX32(5.67128181962), FIX32(6.31375151468), FIX32(7.11536972238), FIX32(8.14434642797), FIX32(9.51436445422), FIX32(11.4300523028), FIX32(14.3006662567), FIX32(19.0811366877), FIX32(28.6362532829), FIX32(57.2899616308), FIX32(5.44374645107e+15), FIX32(-57.2899616308), FIX32(-28.6362532829), FIX32(-19.0811366877), FIX32(-14.3006662567), FIX32(-11.4300523028), FIX32(-9.51436445422), FIX32(-8.14434642797), FIX32(-7.11536972238), FIX32(-6.31375151468), FIX32(-5.67128181962), FIX32(-5.14455401597), FIX32(-4.70463010948), FIX32(-4.33147587428), FIX32(-4.01078093354), FIX32(-3.73205080757), FIX32(-3.48741444384), FIX32(-3.27085261848), FIX32(-3.07768353718), FIX32(-2.90421087768), FIX32(-2.74747741945), FIX32(-2.60508906469), FIX32(-2.47508685342), FIX32(-2.35585236582), FIX32(-2.2460367739), FIX32(-2.14450692051), FIX32(-2.05030384158), FIX32(-1.96261050551), FIX32(-1.88072646535), FIX32(-1.80404775527), FIX32(-1.73205080757), FIX32(-1.66427948235), FIX32(-1.60033452904), FIX32(-1.53986496381), FIX32(-1.48256096851), FIX32(-1.42814800674), FIX32(-1.37638192047), FIX32(-1.32704482162), FIX32(-1.27994163219), FIX32(-1.23489715654), FIX32(-1.19175359259), FIX32(-1.15036840722), FIX32(-1.11061251483), FIX32(-1.07236871002), FIX32(-1.03553031379), FIX32(-1.0), FIX32(-0.965688774807), FIX32(-0.932515086138), FIX32(-0.900404044298), FIX32(-0.869286737816), FIX32(-0.839099631177), FIX32(-0.809784033195), FIX32(-0.781285626507), FIX32(-0.753554050103), FIX32(-0.726542528005), FIX32(-0.70020753821), FIX32(-0.674508516842), FIX32(-0.649407593198), FIX32(-0.624869351909), FIX32(-0.600860619028), FIX32(-0.57735026919), FIX32(-0.554309051453), FIX32(-0.531709431661), FIX32(-0.509525449494), FIX32(-0.487732588566), FIX32(-0.466307658155), FIX32(-0.445228685309), FIX32(-0.42447481621), FIX32(-0.404026225835), FIX32(-0.383864035035), FIX32(-0.363970234266), FIX32(-0.34432761329), FIX32(-0.324919696233), FIX32(-0.305730681459), FIX32(-0.286745385759), FIX32(-0.267949192431), FIX32(-0.249328002843), FIX32(-0.230868191126), FIX32(-0.21255656167), FIX32(-0.194380309138), FIX32(-0.176326980708), FIX32(-0.158384440325), FIX32(-0.140540834702), FIX32(-0.122784560903), FIX32(-0.105104235266), FIX32(-0.0874886635259), FIX32(-0.0699268119435), FIX32(-0.052407779283), FIX32(-0.0349207694917), FIX32(-0.0174550649282) };

const u8 worldMap[ mapWidth ][ mapHeight ] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static Vect2D_u16 position = { 96, 96 };
static u16 directionAngle;
// If not up, then down.
static bool up = false;
// If not right, then left.
static bool right = false;

void gtRaycasterFrame() {
  // todo draw background here
  u16 rayAngle = ( directionAngle + 30 ) % 360;

  // Ray will draw from directionAngle + 30 to directionAngle - 30 (60 degree fov)
  for( u16 column = 0; column != ScreenX; column++ ) {
    up = fix32ToInt( sinLookup[ column ] ) > 0;
    right = fix32ToInt( cosLookup[ column ] ) > 0;

    // Xa and Ya
    Vect2D_s16 step = { 0, 0 };

    // A.x and A.y
    Vect2D_s16 horizInt = { 0, 0 };

    // Calculate A.y and Ya
    if( up ) {
      horizInt.y = ( position.y / TileSize ) * TileSize - 1;
      step.y = -TileSize;
    } else {
      horizInt.y = ( position.y / TileSize ) * TileSize + 64;
      step.y = TileSize;
    }

    // Calculate A.x

  }
}
