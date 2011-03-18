<?php
/**
 * @file
 * Promotions administration menu items.
 */

/**
 * Display created promotions.
 */
function uc_promotions_get_promos(){
  $output = t("<p>Here you can find all created promotions:</p>");
  $output .= _get_promos();
  return $output;
}

/**
 * Display a form to create a promotion.
 */
function uc_promotions_create_promo(){
  $output = t("<p>Fill the form to create a new promotion:</p>");
  $output .= drupal_get_form('uc_promotions_form');
  return $output;
}

/**
 * Display a form to modify a promotion.
 */
function uc_promotions_edit_promo($promo_id = ''){
  $output = t("<p>Fill the form to modify a promotion:</p>");
  $output .= drupal_get_form('uc_promotions_form', $promo_id);
  return $output;
}

/*
 * Returns the form to add a promotion
 */
function uc_promotions_form(&$form_state, $data = NULL){
  $form['nid'] = array(
    '#title' => t('Product'),
    '#description' => t('Attention: to view a product in this list the product should be unpublished'),
    '#type' => 'select',
    '#options' => _get_products(),
    '#required' => true,
    '#multiple' => false,
    '#default_value' => $data['nid'],
  );
  
  $form['price_limit'] = array(
    '#title' => t('Price limit'),
    '#description' => t('Price limit that fires the promotion'),
    '#type' => 'textfield',
    '#required' => true,
    '#size' => 30,
    '#default_value' => $data['price_limit'],
  );
  
  $form['domain'] = array(
    '#title' => t('Domain'),
    '#description' => t('If you have multisite installation you can limit this promotion to fires only for that subdomain'),
    '#type' => 'textfield',
    '#required' => false,
    '#size' => 30,
    '#default_value' => $data['domain'],
  );
  
  $form['status'] = array(
    '#title' => t('Attivo'),
    '#description' => t('The promotion is active'),
    '#type' => 'select',
    '#options' => array('' => '', '1' => 'yes', '0' => 'no'),
    '#required' => true,
    '#default_value' => $data['status'],
  );
  
  $form['promoid'] = array(
    '#title' => t('Promoid'),
    '#type' => 'hidden',
    '#default_value' => $data['promoid'],
  );
  
  if($data['promoid']){
    $form['del'] = array(
      '#title' => t('Deleted'),
      '#description' => t('Delete the promotion'),
      '#type' => 'select',
      '#options' => array('0'=>'no', '1'=>'si'),
      '#required' => false,
    );
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Invia i dati'),
  );
  
  return $form;
}

/*
 * Validates form values 
 */
function uc_promotions_form_validate($form, &$form_state){
  if(!is_numeric($form_state['values']['price_limit'])){
    form_set_error('', t("Price limit require only numerical values"));
  }
}

/*
 * Submits form values 
 */
function uc_promotions_form_submit($form, &$form_state){
  $v = $form_state['values'];
  
  //Delete a promotion
  if($v['del'] == 1){
    $sql = "DELETE FROM {uc_promotionstions} WHERE promoid = %d";
    if(db_query($sql, $v['promoid'])){
      $form_state['redirect'] = 'admin/store/uc_promotionstions';
      drupal_set_message(t('Promotion deleted'), $type = 'status');
    }
  }else{
    //Create or update a promotion
    $update = isset($v['promoid']) && $v['promoid'] != '' ? 'promoid' : array();

    if(drupal_write_record('uc_promotionstions', $form_state['values'], $update)) {
      $form_state['redirect'] = 'admin/store/uc_promotionstions';
      drupal_set_message(t('Promotion created/modified'), $type = 'status');
    }else{
      drupal_set_message(t('An error has occured while processing the request'), $type = 'error');
    }
  }
}


/* 
 * Returns a list of promotions
 */
function _get_promos(){
  $sql = "SELECT * FROM {uc_promotionstions}";
  $result = db_query($sql);
  
  $headers = array();
  $headers[]= t('Promo-id');
  $headers[]= t('Product');
  $headers[]= t('Price limit');
  $headers[]= t('Domain');
  $headers[]= t('Status');
  $headers[]= t('Actions');
  
  $rows = array();
  while($data = db_fetch_object($result)){
    $content = array();
    $product = node_load($data->nid);
    
    $content[]= $data->promoid;
    $content[]= l($product->title, 'node/'.$product->nid);
    $content[]= '&euro; '.number_format($data->price_limit, 2);
    $content[]= $data->domain;
    $content[]= $data->status == '1' ? t('active') : t('not active');
    $content[]= l(t('edit'), 'admin/store/uc_promotions/'.$data->promoid.'/edit');
    $rows[]=$content;
  }
   
  return theme_table($headers, $rows);
}

/*
 * Returns active products
 */
function _get_products(){
  $sql = "SELECT * FROM {node} WHERE type = '%s' AND status = %d ORDER BY '%s'";
  $result = db_query(db_rewrite_sql($sql), "product", 0, 'nid');
  
  $results = array("" => "");
  while($data = db_fetch_object($result)){
    $results[$data->nid] = '['.$data->nid.'] '.$data->title;
  }
  
  return $results;
}