<?php
/**
 *	Función para obtener modalidad a través de la modalidad y la temporada
 *	activas
 */
function get_modalidad_juego_temporada_tid_id_temporada( $tid_Modalidad_Juego, $id_Temporada ) {
	$Modalidad_Juego_Temporada = db_fetch_object( db_query('SELECT * FROM {eSM_Modalidad_Juego_Temporada } AS { mjt } INNER JOIN {eSM_Modalidad_Juego } as{ mj } ON { mjt.id_Modalidad_Juego = mj.id_Modalidad_Juego }
														   WHERE { mj.tid } = %d AND { mjt.id_Temporada } = %d', $tid_Modalidad_Juego, $id_Temporada ) );
		
	return is_null ( $Modalidad_Juego_Temporada ) ? NULL : $Modalidad_Juego_Temporada;		
}
/**
 *	Función para obtener la modalidad de juego en la temporada a través del uid
 */
function get_modalidad_juego_temporada_user	( $user ) {
	$Modalidad_Juego_Temporada = db_fetch_object( db_query( 'SELECT * FROM {eSM_Jugador } as { j } INNER JOIN {eSM_Modalidad_Juego_Temporada } as { mjt } ON { j.id_Modalidad_Juego_Temporada = mjt.id_Modalidad_Juego_temporada }
														   WHERE { j.uid } = %d ', $user->uid ) );
		
	return is_null ( $Modalidad_Juego_Temporada ) ? NULL : $Modalidad_Juego_Temporada;	
}
/**
*	Función para obtener la modalidad de juego a través de el objeto $user
*/
function get_modalidad_juego_user( $user ) {
	$Modalidad_Juego = db_fetch_object ( db_query( 'SELECT { * } FROM {eSM_Modalidad_Juego } AS { mj } INNER JOIN {eSM_Modalidad_Juego_Temporada } AS { mjt } ON { mj.id_Modalidad_Juego = mjt.id_Modalidad_Juego }
												  INNER JOIN {eSM_Jugador } AS { j } ON { mjt.id_Modalidad_Juego_Temporada = j.id_Modalidad_Juego_Temporada }
												  WHERE { j.uid = %d } ', $user->uid ) );

	return is_null ( $Modalidad_Juego ) ? NULL : $Modalidad_Juego;
}
/**
 * Función para obtener todas las modalidades activas
 * en la temporada activa.
 * Devuelve NULL si no hay temporada activa
 * o si no hay ninguna modalidad activa.
 * o devuelve un arreglo de objetos conteniendo
 * todas las modalidades activas en la temporada.
 */
function get_modalidades_juego_temporada_activas ( ) {
	$Temporada = temporada_activa( );   //Obtiene la temporada activa actual
	
	if ( isset( $Temporada ) ) {
		$result = db_query ( 'SELECT * FROM {eSM_Modalidad_Juego_Temporada}
							WHERE id_Temporada = %d', $Temporada->id_Temporada );
		
		$count = 0;
		while ( $Modalidad_activa = db_fetch_object ( $result ) ) {
			++$count;
			$Modalidad = get_modalidad_juego_temporada ( $Modalidad_activa->id_Modalidad_Juego_Temporada );
			if ( isset( $Modalidad ) )
				$Modalidades[  ] = $Modalidad;
		}
		if ( $count == 0 )
			return NULL;
		else
			return $Modalidades;
	}
	else {
		return NULL;
	}
}