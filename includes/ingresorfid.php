<?php
require_once("_db.php"); 
global $conexion; 
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (!$conexion) {
        die("Conexión fallida: " . mysqli_connect_error());
    }

    if (isset($_POST['uid']) && isset($_POST['Area'])) {
        $uid =  $_POST['uid']; 
        $Area = $_POST['Area']; 

        $consultaUsuario = "SELECT usuario.idUsuario, usuario.TagRFID, usuario.idRol, 
            roles.nombreRol
            FROM usuario
            LEFT JOIN roles ON usuario.idRol = roles.idRol
            WHERE usuario.TagRFID = '$uid' ";
        $consultaArea ="SELECT * from areas WHERE idAreas = '$Area'";

        $result_usuario = mysqli_query($conexion, $consultaUsuario);
        $result_Area = mysqli_query($conexion, $consultaArea);
        if (mysqli_num_rows($result_usuario) > 0 && mysqli_num_rows($result_Area) > 0) {
            $row_usuario = mysqli_fetch_assoc($result_usuario);
            $row_area = mysqli_fetch_assoc($result_Area);

            $permisosRoles = $row_area['permisosRoles']; 

            // Enviar arduino
            echo "Tag RFID: " . $row_usuario['TagRFID'] . "\n";
            echo "ID Usuario: " . $row_usuario['idUsuario'] . "\n";
            echo "Rol Usuario: " . $row_usuario['nombreRol'] . "\n";
            echo "Permisos del Área: " . $permisosRoles . "\n";

            if ($permisosRoles === 'Libre' || strpos($permisosRoles, $row_usuario['nombreRol']) !== false) {
                // Insertar en la base de datos
                $sql = "INSERT INTO accesos_logs (idAccesoLog, idUsuario, idArea, tipoAcesso)
                VALUES (NULL, (SELECT idUsuario FROM usuario WHERE TagRFID = '$uid'), '$Area', 'Entrada');";
                if ($conexion->query($sql) === TRUE) {
                    
                    echo "Acceso permitido al área";
                } else {
                    echo "Error: " . $sql . "<br>" . $conn->error;
}
                
            } else {
                echo "Acceso denegado al área.";
            }
        } else {
            echo "No se encontraron resultados para el UID o el Área especificados.";
        }
    }    
}
?>
