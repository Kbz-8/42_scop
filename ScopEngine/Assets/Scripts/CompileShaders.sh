mkdir -p "$1/Assets/Shaders/Build/"

VERTEX="$1/Assets/Shaders/Vertex.nzsl"
FRAGMENT="$1/Assets/Shaders/DefaultFragment.nzsl"

VERTEX_SPV="$1/Assets/Shaders/Build/Vertex.spv"
FRAGMENT_SPV="$1/Assets/Shaders/Build/DefaultFragment.spv"

if [ ! -f "$VERTEX_SPV" ]; then
	$1/Assets/Vendors/nzslc.x86_64 --compile=spv $VERTEX -o $1/Assets/Shaders/Build
	echo "Compiling vertex shader"
fi

if [ ! -f "$FRAGMENT_SPV" ]; then
	$1/Assets/Vendors/nzslc.x86_64 --compile=spv $FRAGMENT -o $1/Assets/Shaders/Build
	echo "Compiling fragment shader"
fi
