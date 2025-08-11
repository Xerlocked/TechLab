#include "stdafx.h"
#include "UCameraComponent.h"

void UCameraComponent::Update() {
	aspectRatio = SCR_WIDTH / (float)SCR_HEIGHT;
	const float speed = 2.0f;
	auto loc = GetRelativeLocation();
	FVector movement = FVector::Zero;
	if (allowKeyboardInput)
	{
		if ( Input::Instance()->IsKeyDown(DIK_A) ) {
			movement -= Right();
		}
		if ( Input::Instance()->IsKeyDown(DIK_D) ) {
			movement += Right();
		}
		if ( Input::Instance()->IsKeyDown(DIK_W) ) {
			movement += Front();
		}
		if ( Input::Instance()->IsKeyDown(DIK_S) ) {
			movement -= Front();
		}
		if ( Input::Instance()->IsKeyDown(DIK_E) ) {
			movement += Up();
		}
		if ( Input::Instance()->IsKeyDown(DIK_Q) ) {
			movement -= Up();
		}
	}
	SetRelativeLocation(loc + movement * Time::GetDeltaTime() * speed);
	if ( Input::Instance()->IsMouseButtonDown(1) ) {
		int dx, dy;
		Input::Instance()->GetMouseDelta(dx, dy);
		auto rot = GetRelativeRotation();
		SetRelativeRotation(rot - FVector(degToRad(dy) * mouseSensitive, degToRad(dx) * mouseSensitive, 0));

		//RelativeRotation.y -= degToRad(dx) * mouseSensitive;
		//RelativeRotation.x -= degToRad(dy) * mouseSensitive;
	}
	//OutputDebugString((std::to_wstring(dx) + L"\n").c_str());
}

void UCameraComponent::Render() {

	
}

void UCameraComponent::RenderUI()
{
	//ImGui::Text(("position: " + static_cast<std::string>(RelativeLocation)).c_str());
	//ImGui::Text(("rotation: " + static_cast<std::string>(RelativeRotation)).c_str());
	ImGui::Checkbox("Orthogonal", &orthogonal);
	if (!orthogonal)
		ImGui::SliderFloat("FOV", &fieldOfView, 10.f, 90.f);
	else
		ImGui::SliderFloat("Scale", &orthoScale, 0.01f, 1.0f);

	FVector vec = GetRelativeLocation();
	float loc[3] = { vec.x, vec.y, vec.z };
	//ImGui::SliderFloat3("position", loc, -50.f, 50.f);
	ImGui::DragFloat3("position", loc, 0.1f);
	SetRelativeLocation(FVector(loc[0], loc[1], loc[2]));

	vec = GetRelativeRotation();
	float rot[3] = { vec.x, vec.y, vec.z };
	//ImGui::SliderFloat3("rotation", rot, -M_PI, M_PI);
	ImGui::DragFloat3("rotation", rot, 0.1f);
	SetRelativeRotation(FVector(rot[0], rot[1], rot[2]));
	//ImGui::DragFloat3("position", &RelativeLocation.x, 0.1f);
	//ImGui::DragFloat3("rotation", &RelativeRotation.x, 0.1f);

}

FMatrix UCameraComponent::View()
{
	return GetRelativeTransform().Inverse();





	//FVector eye = GetRelativeLocation();
	//FVector at = GetRelativeLocation() + FVector(0, 0, 1);
	//FVector up = FVector(0, 1, 0);

	////eye = FVector(eye.x, eye.y, -eye.z);
	////at = FVector(at.x, at.y, -at.z);
	////up = FVector(up.x, up.y, -up.z);
	//FVector n = (eye - at) / (eye - at).Magnitude();
	//FVector u = up.Cross(n) / up.Cross(n).Magnitude();
	//FVector v = n.Cross(u);

	//FMatrix view = {
	//	u.x, v.x, n.x, 0,
	//	u.y, v.y, n.y, 0,
	//	u.z, v.z, n.z, 0,
	//	-eye.x, -eye.y, -eye.z, 1
	//};


	////view = view.Transpose();

	//return view;



	//FMatrix Trnasform = GetComponentTransform();
	//FVector4 u = FVector4((FVector4(1, 0, 0, 0) * Trnasform).xyz(), 1.f);
	//FVector4 v = FVector4((FVector4(0, 1, 0, 0) * Trnasform).xyz(), 1.f);
	//FVector4 n = FVector4((FVector4(0, 0, 1, 0) * Trnasform).xyz(), 1.f);

	//FMatrix Rotation = { u,v,n,FVector4(0,0,0,1) };

	FVector T = GetComponentLocation();
	FMatrix mat =
	{
		Right().x, Up().x, Front().x, 0,
		Right().y, Up().y, Front().y, 0,
		Right().z, Up().z, Front().z, 0,
		-T.Dot(Right()), -T.Dot(Up()), -T.Dot(Front()), 1
	};
	mat = mat.Transpose();
	
	//mat = this->GetComponentTransform().Transpose();


	//return FMatrix::Identity;
	return mat;
}

FMatrix UCameraComponent::Projection() {
	if ( orthogonal )
		return OrthgonalProjection();
	else
		return PerspectiveProjection();
}

FMatrix UCameraComponent::OrthgonalProjection() {
	float zRange = farDistance - nearDistance;
	const float scale = orthoScale;
	return FMatrix({ 
		scale / aspectRatio, 0.f, 0.f, 0.f, 
		0.f, scale, 0.f, 0.f,
		0.f, 0.f, 1.f / zRange, - nearDistance / zRange,
		0.f, 0.f, 0.f, 1.f
	});
}

//FMatrix UCameraComponent::InverseTransformation() {
//	FMatrix m = FMatrix::Translate(-RelativeLocation.x, -RelativeLocation.y, -RelativeLocation.z);
//	m = m * FMatrix::RotateZ(-RelativeRotation.z);
//	m = m * FMatrix::RotateY(-RelativeRotation.y);
//	m = m * FMatrix::RotateX(-RelativeRotation.x);
//	//m = m * FMatrix::Scale(1 / RelativeScale3D.x, 1 / RelativeScale3D.y, 1 / RelativeScale3D.z);
//	return m;
//}

FMatrix UCameraComponent::PerspectiveProjection() {
	float yScale = 1.0f / tanf(degToRad(fieldOfView * 0.5f)); // cot(FOV/2)
	float xScale = yScale / aspectRatio;
	float zRange = farDistance - nearDistance;

	return FMatrix({
		 xScale,  0.0f,   0.0f,                         0.0f ,
		 0.0f,    yScale, 0.0f,                         0.0f,
		 0.0f,    0.0f,   farDistance / zRange,                1.0f ,
		 0.0f,    0.0f,  -nearDistance * farDistance / zRange,        0.0f 
		});
	//return FMatrix({
	//	1 / (t * aspectRatio), 0, 0, 0,
	//	0, 1 / t, 0, 0,
	//	0, 0, (farDistance + nearDistance) / (farDistance - nearDistance), -farDistance * nearDistance / (farDistance - nearDistance),
	//	0, 0, 1, 0
	//	});
}
