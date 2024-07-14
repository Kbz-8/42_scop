mkdir -p "$1/Assets/Shaders/Build/"

VERTEX="$1/Assets/Shaders/Vertex.nzsl"
FRAGMENT_DEFAULT="$1/Assets/Shaders/DefaultFragment.nzsl"
FRAGMENT_BASIC="$1/Assets/Shaders/BasicFragment.nzsl"

VERTEX_SPV="$1/Assets/Shaders/Build/Vertex.spv"
FRAGMENT_DEFAULT_SPV="$1/Assets/Shaders/Build/DefaultFragment.spv"
FRAGMENT_BASIC_SPV="$1/Assets/Shaders/Build/BasicFragment.spv"

if [ ! -f "$VERTEX_SPV" ]; then
	$1/Assets/Vendors/nzslc.x86_64 --compile=spv $VERTEX -o $1/Assets/Shaders/Build
	echo "Compiling vertex shader"
fi

if [ ! -f "$FRAGMENT_DEFAULT_SPV" ]; then
	$1/Assets/Vendors/nzslc.x86_64 --compile=spv $FRAGMENT_DEFAULT -o $1/Assets/Shaders/Build
	echo "Compiling default fragment shader"
fi

if [ ! -f "$FRAGMENT_BASIC_SPV" ]; then
	$1/Assets/Vendors/nzslc.x86_64 --compile=spv $FRAGMENT_BASIC -o $1/Assets/Shaders/Build
	echo "Compiling basic fragment shader"
fi
