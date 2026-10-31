#include "../lua_library.hpp"
#include "../lua_module.hpp"
#include "../wrappers/imdrawlist.hpp"


namespace YLP::LuaJIT
{
	class LuaImDrawListLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L)
		{
			// clang-format off

			/*@ylp.enum ImDrawFlags
			* field None<integer> 
			* field Closed<integer> PathStroke(), AddPolyline(): specify that shape should be closed (Important: this is always == 1 for legacy reason)
			* field RoundCornersTopLeft<integer> AddRect(), AddRectFilled(), PathRect(): enable rounding top-left corner only (when rounding > 0.0f, we default to all corners). Was 0x01.
			* field RoundCornersTopRight<integer> AddRect(), AddRectFilled(), PathRect(): enable rounding top-right corner only (when rounding > 0.0f, we default to all corners). Was 0x02.
			* field RoundCornersBottomLeft<integer> AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-left corner only (when rounding > 0.0f, we default to all corners). Was 0x04.
			* field RoundCornersBottomRight<integer> AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-right corner only (when rounding > 0.0f, we default to all corners). Wax 0x08.
			* field RoundCornersNone<integer> AddRect(), AddRectFilled(), PathRect(): disable rounding on all corners (when rounding > 0.0f). This is NOT zero, NOT an implicit flag!
			* field RoundCornersTop<integer> 
			* field RoundCornersBottom<integer> 
			* field RoundCornersLeft<integer> 
			* field RoundCornersRight<integer> 
			* field RoundCornersAll<integer> 
			* field RoundCornersDefault_<integer> Default to ALL corners if none of the _RoundCornersXX flags are specified.
			* field RoundCornersMask_<integer> 
			@*/
			L.new_enum("ImDrawFlags",
				"None", ImDrawFlags_None,
				"Closed", ImDrawFlags_Closed,
				"RoundCornersTopLeft", ImDrawFlags_RoundCornersTopLeft,
				"RoundCornersTopRight", ImDrawFlags_RoundCornersTopRight,
				"RoundCornersBottomLeft", ImDrawFlags_RoundCornersBottomLeft,
				"RoundCornersBottomRight", ImDrawFlags_RoundCornersBottomRight,
				"RoundCornersNone", ImDrawFlags_RoundCornersNone,
				"RoundCornersTop", ImDrawFlags_RoundCornersTop,
				"RoundCornersBottom", ImDrawFlags_RoundCornersBottom,
				"RoundCornersLeft", ImDrawFlags_RoundCornersLeft,
				"RoundCornersRight", ImDrawFlags_RoundCornersRight,
				"RoundCornersAll", ImDrawFlags_RoundCornersAll,
				"RoundCornersDefault_", ImDrawFlags_RoundCornersDefault_,
				"RoundCornersMask_", ImDrawFlags_RoundCornersMask_);

			/*@ylp.class ImDrawList
			* description
				Provides access to ImGui primitives.

				__NOTE:__ You can not construct this class from Lua. You can get an instance by calling the appropriate

				ImGui function: [GetWindowDrawList](lua://ImGui.GetWindowDrawList), [GetBackgroundDrawList](lua://ImGui.GetBackgroundDrawList), [GetForegroundDrawList](lua://ImGui.GetForegroundDrawList)

			* method AddText
			* param pos<ImVec2>
			* param col<integer>
			* param text<string>

			* method AddText
			* param fontSize<number>
			* param pos<ImVec2>
			* param col<integer>
			* param text<string>
			* param wrapWidth<number?>

			* method AddLine
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param col<integer>
			* param thickness<number?>

			* method AddPolyLine
			* param points<{ [integer]: ImVec2 }> An array of ImVec2 objects. **MUST** be a contiguous ImVec2 array, else the function will throw.
			* param numPoints<integer>
			* param col<integer>
			* param flags<integer> [ImDrawFlags](lua://ImDrawFlags)
			* param thickness<number>

			* method AddRect
			* param pMin<ImVec2>
			* param pMax<ImVec2>
			* param col<integer>
			* param rounding<number?>

			* method AddRect
			* param pMin<ImVec2>
			* param pMax<ImVec2>
			* param col<integer>
			* param rounding<number>
			* param flags<integer?> Optional [ImDrawFlags](lua://ImDrawFlags)

			* method AddRect
			* param pMin<ImVec2>
			* param pMax<ImVec2>
			* param col<integer>
			* param rounding<number>
			* param flags<integer> [ImDrawFlags](lua://ImDrawFlags)
			* param thickness<number?>

			* method AddRectFilled
			* param pMin<ImVec2>
			* param pMax<ImVec2>
			* param col<integer>
			* param rounding<number?>

			* method AddRectFilled
			* param pMin<ImVec2>
			* param pMax<ImVec2>
			* param col<integer>
			* param rounding<number>
			* param flags<integer?> Optional [ImDrawFlags](lua://ImDrawFlags)

			* method AddCircle
			* param center<ImVec2>
			* param radius<number>
			* param col<integer>
			* param numSegments<integer?>

			* method AddCircle
			* param center<ImVec2>
			* param radius<number>
			* param col<integer>
			* param numSegments<integer>
			* param thickness<number?>

			* method AddCircleFilled
			* param center<ImVec2>
			* param radius<number>
			* param col<integer>
			* param numSegments<integer?>

			* method AddNgon
			* param center<ImVec2>
			* param radius<number>
			* param col<integer>
			* param numSegments<integer>
			* param thickness<number?>

			* method AddNgonFilled
			* param center<ImVec2>
			* param radius<number>
			* param col<integer>
			* param numSegments<integer>

			* method AddQuad
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param p4<ImVec2>
			* param col<integer>
			* param thickness<number?>

			* method AddQuadFilled
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param p4<ImVec2>
			* param col<integer>

			* method AddBezierCubic
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param p4<ImVec2>
			* param col<integer>
			* param thickness<number>
			* param numSegments<integer?>

			* method AddBezierQuadratic
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param col<integer>
			* param thickness<number>
			* param numSegments<integer?>

			* method AddTriangle
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param col<integer>
			* param thickness<number?>

			* method AddTriangleFilled
			* param p1<ImVec2>
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param col<integer>
			
			* method PathClear
			*
			
			* method PathLineTo
			* param pos<ImVec2>
			
			* method PathStroke
			* param col<integer>
			* param flags<integer?>
			
			* method PathStroke
			* param col<integer>
			* param flags<integer>
			* param thickness<number?>
			
			* method PathArcTo
			* param center<ImVec2>
			* param radius<number>
			* param aMin<number>
			* param aMax<number>
			* param numSegments<integer?>
			
			* method PathArcToFast
			* param center<ImVec2>
			* param radius<number>
			* param aMinOf12<integer> Minimum indices into the 12-step circle approximation
			* param aMaxOf12<integer> Maximum indices into the 12-step circle approximation
			
			* method PathFillConcave
			* param col<integer>
			
			* method PathRect
			* param rectMin<ImVec2>
			* param rectMax<ImVec2>
			* param rounding<number?>
			
			* method PathRect
			* param rectMin<ImVec2>
			* param rectMax<ImVec2>
			* param rounding<number>
			* param flags<integer?>
			
			* method PathFillConvex
			* param col<integer>
			
			* method PathBezierCubicCurveTo
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param p4<ImVec2>
			* param numSegments<integer?>
			
			* method PathBezierQuadraticCurveTo
			* param p2<ImVec2>
			* param p3<ImVec2>
			* param numSegments<integer?>
			
			* method PathEllipticalArcTo
			* param center<ImVec2>
			* param radius<ImVec2>
			* param rot<number>
			* param aMin<number>
			* param aMax<number>
			* param numSegments<integer?>
			
			* method PushClipRect
			* param min<ImVec2>
			* param max<ImVec2>
			* param intersectWithCurrentClipRect<boolean?>
			
			* method PopClipRect
			*
			@*/
			L.new_usertype<LuaImDrawList>("ImDrawList", sol::no_constructor,
				"AddLine",				      &LuaImDrawList::AddLine,
				"AddPolyline",			      &LuaImDrawList::AddPolyline,
				"AddCircleFilled",		      &LuaImDrawList::AddCircleFilled,
				"AddNgon",				      &LuaImDrawList::AddNgon,
				"AddNgonFilled",		      &LuaImDrawList::AddNgonFilled,
				"AddBezierCubic",		      &LuaImDrawList::AddBezierCubic,
				"AddBezierQuadratic",	      &LuaImDrawList::AddBezierQuadratic,
				"AddQuad",				      &LuaImDrawList::AddQuad,
				"AddQuadFilled",		      &LuaImDrawList::AddQuadFilled,
				"AddTriangle",                &LuaImDrawList::AddTriangle,
				"AddTriangleFilled",          &LuaImDrawList::AddTriangleFilled,
				"AddRectFilledMultiColor",    &LuaImDrawList::AddRectFilledMultiColor,
				"PathClear",			      &LuaImDrawList::PathClear,
				"PathLineTo",			      &LuaImDrawList::PathLineTo,
				"PathArcTo",			      &LuaImDrawList::PathArcTo,
				"PathArcToFast",		      &LuaImDrawList::PathArcToFast,
				"PathFillConcave",		      &LuaImDrawList::PathFillConcave,
				"PathFillConvex",		      &LuaImDrawList::PathFillConvex,
				"PathBezierCubicCurveTo",     &LuaImDrawList::PathBezierCubicCurveTo,
				"PathBezierQuadraticCurveTo", &LuaImDrawList::PathBezierCubicCurveTo,
				"PathEllipticalArcTo",		  &LuaImDrawList::PathEllipticalArcTo,
				"PushClipRect",				  &LuaImDrawList::PushClipRect,
				"PopClipRect",				  &LuaImDrawList::PopClipRect,

				"PathStroke",    sol::overload(&LuaImDrawList::PathStroke,    &LuaImDrawList::PathStroke1),
				"PathRect",      sol::overload(&LuaImDrawList::PathRect,      &LuaImDrawList::PathRect1),
				"AddText",       sol::overload(&LuaImDrawList::AddText,       &LuaImDrawList::AddText1),
				"AddRectFilled", sol::overload(&LuaImDrawList::AddRectFilled, &LuaImDrawList::AddRectFilled1),
				"AddCircle",     sol::overload(&LuaImDrawList::AddCircle,	  &LuaImDrawList::AddCircle1),
				"AddRect",       sol::overload(&LuaImDrawList::AddRect,		  &LuaImDrawList::AddRect1, &LuaImDrawList::AddRect2)
			);
		}
	};

	LuaImDrawListLib _LuaImDrawListLib;
}
